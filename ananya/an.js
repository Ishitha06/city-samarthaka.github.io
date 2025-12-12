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


/* TOGGLE */

// Fast page fade-in
document.addEventListener("DOMContentLoaded", function () {
    document.body.classList.add("page-loaded");
});

// Show / hide the Union–Find zoning code block
function toggleZoningCode() {
    const block = document.getElementById("zoning-code-block");
    if (!block) return;
    if (block.style.display === "none" || block.style.display === "") {
        block.style.display = "block";
    } else {
        block.style.display = "none";
    }
}
function toggleDcrCode() {
const block = document.getElementById("dcr-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
    block.style.display = "block";
} else {
    block.style.display = "none";
}
}

function togglePhasingCode() {
const block = document.getElementById("phasing-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
    block.style.display = "block";
} else {
    block.style.display = "none";
}
}

function togglePatrolCode() {
const block = document.getElementById("patrol-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
    block.style.display = "block";
} else {
    block.style.display = "none";
}
}
function toggleHeapCode() {
  const block = document.getElementById("heap-code-block");
  if (!block) return;
  if (block.style.display === "none" || block.style.display === "") {
    block.style.display = "block";
  } else {
    block.style.display = "none";
  }
}
function toggleAnimalHashCode() {
const block = document.getElementById("animal-hash-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
block.style.display = "block";
} else {
block.style.display = "none";
}
}
function toggleBITCode() {
const block = document.getElementById("bit-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
block.style.display = "block";
} else {
block.style.display = "none";
}
}
function toggleBMCode() {
const block = document.getElementById("bm-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
block.style.display = "block";
} else {
block.style.display = "none";
}
}

function toggleSegTreeCode() {
const block = document.getElementById("seg-code-block");
if (!block) return;
if (block.style.display === "none" || block.style.display === "") {
block.style.display = "block";
} else {
block.style.display = "none";
}
}    