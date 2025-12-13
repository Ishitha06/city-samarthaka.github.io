
// Enhanced JavaScript with improved animations
document.addEventListener('DOMContentLoaded', function() {
  // Smooth scrolling
  document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
      e.preventDefault();
      const target = document.querySelector(this.getAttribute('href'));
      if (target) {
        target.scrollIntoView({
          behavior: 'smooth',
          block: 'start'
        });
      }
    });
  });

  // Enhanced table row hover effects
  const tableRows = document.querySelectorAll('table tr');
  tableRows.forEach(row => {
    row.addEventListener('mouseenter', function() {
      this.style.transition = 'all 0.4s cubic-bezier(0.25, 0.46, 0.45, 0.94)';
    });
  });

  // Enhanced button interactions
  const buttons = document.querySelectorAll('.btn, .back-btn');
  buttons.forEach(button => {
    button.addEventListener('mousedown', function() {
      this.style.transform = 'scale(0.92)';
    });
    button.addEventListener('mouseup', function() {
      this.style.transform = 'translateY(-5px) scale(1.03)';
    });
    button.addEventListener('mouseleave', function() {
      this.style.transform = '';
    });
  });

  // Section counter animation
  const sectionHeadings = document.querySelectorAll('.section-heading');
  sectionHeadings.forEach((heading, index) => {
    heading.style.counterIncrement = 'section-counter';
    heading.style.position = 'relative';
    heading.style.paddingLeft = '70px';
    
    const number = document.createElement('span');
    number.textContent = (index + 1) + '. ';
    number.style.position = 'absolute';
    number.style.left = '0';
    number.style.top = '50%';
    number.style.transform = 'translateY(-50%)';
    number.style.fontSize = '2.8rem';
    number.style.fontWeight = '800';
    number.style.color = '#1a3c27';
    number.style.opacity = '0.25';
    number.style.width = '60px';
    number.style.textAlign = 'center';
    heading.insertBefore(number, heading.firstChild);
  });

  // Add subtle fade-in animation to content
  const contentElements = document.querySelectorAll('h2, h3, p, table');
  contentElements.forEach((el, index) => {
    el.style.opacity = '0';
    el.style.transform = 'translateY(20px)';
    el.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
    
    setTimeout(() => {
      el.style.opacity = '1';
      el.style.transform = 'translateY(0)';
    }, 100 + index * 50);
  });

  // Add scroll progress indicator
  const progressBar = document.createElement('div');
  progressBar.style.position = 'fixed';
  progressBar.style.top = '0';
  progressBar.style.left = '0';
  progressBar.style.width = '0%';
  progressBar.style.height = '4px';
  progressBar.style.background = 'linear-gradient(90deg, #1a3c27, #2e7d32)';
  progressBar.style.zIndex = '9999';
  progressBar.style.transition = 'width 0.3s ease';
  document.body.appendChild(progressBar);

  window.addEventListener('scroll', () => {
    const winScroll = document.body.scrollTop || document.documentElement.scrollTop;
    const height = document.documentElement.scrollHeight - document.documentElement.clientHeight;
    const scrolled = (winScroll / height) * 100;
    progressBar.style.width = scrolled + '%';
  });

  // Add "scroll to top" button
  const scrollToTopBtn = document.createElement('button');
  scrollToTopBtn.innerHTML = '↑';
  scrollToTopBtn.style.position = 'fixed';
  scrollToTopBtn.style.bottom = '30px';
  scrollToTopBtn.style.right = '30px';
  scrollToTopBtn.style.width = '50px';
  scrollToTopBtn.style.height = '50px';
  scrollToTopBtn.style.borderRadius = '50%';
  scrollToTopBtn.style.background = 'linear-gradient(135deg, #1a3c27, #14321f)';
  scrollToTopBtn.style.color = 'white';
  scrollToTopBtn.style.border = 'none';
  scrollToTopBtn.style.fontSize = '1.5rem';
  scrollToTopBtn.style.cursor = 'pointer';
  scrollToTopBtn.style.boxShadow = '0 5px 15px rgba(0,0,0,0.3)';
  scrollToTopBtn.style.opacity = '0';
  scrollToTopBtn.style.transition = 'opacity 0.3s ease, transform 0.3s ease';
  scrollToTopBtn.style.zIndex = '999';
  document.body.appendChild(scrollToTopBtn);

  scrollToTopBtn.addEventListener('click', () => {
    window.scrollTo({
      top: 0,
      behavior: 'smooth'
    });
  });

  scrollToTopBtn.addEventListener('mouseenter', () => {
    scrollToTopBtn.style.transform = 'scale(1.1)';
  });

  scrollToTopBtn.addEventListener('mouseleave', () => {
    scrollToTopBtn.style.transform = 'scale(1)';
  });

  window.addEventListener('scroll', () => {
    if (window.scrollY > 300) {
      scrollToTopBtn.style.opacity = '1';
    } else {
      scrollToTopBtn.style.opacity = '0';
    }
  });
});
