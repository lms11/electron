// Simple renderer script for Electron Android
document.addEventListener('DOMContentLoaded', function() {
  const info = document.getElementById('info');
  if (info) {
    info.textContent = 'Running on Android!';
  }
});