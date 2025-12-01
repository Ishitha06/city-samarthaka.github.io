// ---------------------------------------------------------
// 1. Smooth Scroll for Navigation Links
// ---------------------------------------------------------
document.querySelectorAll('.nav-links a[href^="#"]').forEach(link => {
    link.addEventListener('click', function (e) {
        e.preventDefault();
        document.querySelector(this.getAttribute('href')).scrollIntoView({
            behavior: "smooth"
        });
    });
});


// ---------------------------------------------------------
// 2. "Explore Our Vision" Button Scroll
// ---------------------------------------------------------
const exploreBtn = document.querySelector(".cta-btn");
if (exploreBtn) {
    exploreBtn.addEventListener("click", () => {
        document.querySelector("#about").scrollIntoView({ behavior: "smooth" });
    });
}


// ---------------------------------------------------------
// 3. Fade-in Animations for Sections (on scroll)
// ---------------------------------------------------------
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add("visible");
        }
    });
});

document.querySelectorAll("section, .blueprint-section, .contrib-card").forEach(section => {
    section.classList.add("fade");
    observer.observe(section);
});


// ---------------------------------------------------------
// 4. Contribution Cards Hover Animation
// ---------------------------------------------------------
document.querySelectorAll(".contrib-card").forEach(card => {
    card.addEventListener("mouseenter", () => card.classList.add("hover"));
    card.addEventListener("mouseleave", () => card.classList.remove("hover"));
});


// ---------------------------------------------------------
// 5. View Work Buttons Logic
// (Already has correct hrefs; JS only ensures smooth feel)
// ---------------------------------------------------------
document.querySelectorAll(".view-btn").forEach(btn => {
    btn.addEventListener("click", () => {
        btn.classList.add("active-click");
        setTimeout(() => btn.classList.remove("active-click"), 300);
    });
});


// ---------------------------------------------------------
// 6. Back-to-Team button handling (for inner pages)
// ---------------------------------------------------------
const backBtn = document.querySelector(".back-btn");
if (backBtn) {
    backBtn.addEventListener("click", function (e) {
        e.preventDefault();

        // Go back to main homepage no matter the folder level
        window.location.href = "../index.html";
    });
}
