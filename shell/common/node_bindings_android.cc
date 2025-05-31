#include "shell/common/node_bindings.h"

#include <android/log.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "base/logging.h"
#include "base/posix/eintr_wrapper.h"

#define LOG_TAG "NodeBindingsAndroid"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace electron {

class NodeBindingsAndroid : public NodeBindings {
 public:
  explicit NodeBindingsAndroid(BrowserEnvironment browser_env)
      : NodeBindings(browser_env), epoll_fd_(-1) {}

  ~NodeBindingsAndroid() override {
    if (epoll_fd_ != -1) {
      close(epoll_fd_);
    }
  }

  void PollEvents() override {
    if (epoll_fd_ == -1) {
      LOGE("PollEvents called with invalid epoll_fd_");
      return;
    }

    int timeout = uv_backend_timeout(uv_loop_);
    if (timeout < 0) {
      timeout = 0;
    }

    // Wait for events on the epoll file descriptor
    struct epoll_event ev;
    int result = HANDLE_EINTR(epoll_wait(epoll_fd_, &ev, 1, timeout));
    
    if (result == -1) {
      LOGE("epoll_wait failed: %s", strerror(errno));
      return;
    }

    if (result > 0) {
      LOGI("Received epoll event, running UV loop");
    }

    // Always run the UV loop to handle timers and other events
    uv_run(uv_loop_, UV_RUN_NOWAIT);
  }

  void WakeupMainThread() override {
    if (epoll_fd_ != -1) {
      // Write to the eventfd to wake up epoll_wait
      uint64_t value = 1;
      ssize_t ret = HANDLE_EINTR(write(epoll_fd_, &value, sizeof(value)));
      if (ret != sizeof(value)) {
        LOGE("Failed to write to eventfd: %s", strerror(errno));
      }
    }
  }

 private:
  void Initialize() override {
    NodeBindings::Initialize();
    
    // Create an eventfd for waking up the main thread
    epoll_fd_ = eventfd(0, EFD_CLOEXEC);
    if (epoll_fd_ == -1) {
      LOGE("Failed to create eventfd: %s", strerror(errno));
      return;
    }

    // Add the eventfd to epoll for monitoring
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = epoll_fd_;
    
    int epoll_ctl_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_ctl_fd == -1) {
      LOGE("Failed to create epoll: %s", strerror(errno));
      close(epoll_fd_);
      epoll_fd_ = -1;
      return;
    }

    if (epoll_ctl(epoll_ctl_fd, EPOLL_CTL_ADD, epoll_fd_, &ev) == -1) {
      LOGE("Failed to add eventfd to epoll: %s", strerror(errno));
      close(epoll_ctl_fd);
      close(epoll_fd_);
      epoll_fd_ = -1;
      return;
    }

    close(epoll_ctl_fd);
    LOGI("Android Node.js event loop initialized successfully");
  }

  int epoll_fd_;
};

// Factory method
std::unique_ptr<NodeBindings> NodeBindings::Create(BrowserEnvironment browser_env) {
  auto bindings = std::make_unique<NodeBindingsAndroid>(browser_env);
  bindings->Initialize();
  return std::move(bindings);
}

}  // namespace electron