/* ---------------------------------------
   1. Fade in page on load
----------------------------------------*/
document.addEventListener("DOMContentLoaded", () => {
    document.body.classList.add("page-loaded");
});


/* ---------------------------------------
   2. Animate section headings on scroll
----------------------------------------*/
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add("visible");
        }
    });
}, { threshold: 0.2 });

document.querySelectorAll(".section-heading").forEach(section => {
    section.classList.add("fade-section");
    observer.observe(section);
});


/* ---------------------------------------
   3. Back to Team Button – Ripple + Navigation
----------------------------------------*/
const backBtn = document.querySelector(".back-btn");
if (backBtn) {

    // Smooth ripple click feedback
    backBtn.addEventListener("click", (e) => {
        e.preventDefault();

        backBtn.classList.add("clicked");

        setTimeout(() => {
            window.location.href = "../index.html";
        }, 250);
    });
}
