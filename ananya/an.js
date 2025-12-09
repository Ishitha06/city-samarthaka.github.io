/* =========================================================
   1. Fast Page Fade-in
========================================================= */
document.addEventListener("DOMContentLoaded", () => {
    document.body.classList.add("page-loaded");
});



/* ---------------------------------------
   2. Fast Scroll Animations
----------------------------------------*/
const animatedItems = document.querySelectorAll(
    "h1, h2, h3, h4, p, img, .section-heading, .card, .box, .container, .content-box, .efficiency-box"
);

const scrollObserver = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add("show");
            scrollObserver.unobserve(entry.target); // animate once
        }
    });
}, { threshold: 0.15 });

animatedItems.forEach(item => {
    item.classList.add("animate-smooth");
    scrollObserver.observe(item);
});




/* =========================================================
   3. Back Button Ripple + Fade-out Transition
========================================================= */
const backBtn = document.querySelector(".back-btn");

if (backBtn) {
    backBtn.addEventListener("click", (e) => {
        e.preventDefault();

        backBtn.classList.remove("clicked");
        void backBtn.offsetWidth; // Restart animation
        backBtn.classList.add("clicked");

        document.body.classList.add("fade-out");

        setTimeout(() => {
            window.location.href = "../index.html";
        }, 200);
    });
}
