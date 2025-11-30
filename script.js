// Smooth scrolling
document.querySelectorAll('a[href^="#"]').forEach(link => {
    link.addEventListener("click", function (e) {
        e.preventDefault();
        document.querySelector(this.getAttribute("href")).scrollIntoView({
            behavior: "smooth"
        });
    });
});

// Disable all links inside .tab-wrapper
document.querySelectorAll('.tab-wrapper a').forEach(tab => {
    tab.addEventListener('click', function(event) {
        event.preventDefault(); // stops navigation
    });
});
