js 

/* =========================================================
   1. Page Load & Animations
========================================================= */
document.addEventListener("DOMContentLoaded", () => {
    document.body.classList.add("page-loaded");
});

const animatedItems = document.querySelectorAll(
    "h1, h2, h3, h4, p, img, .section-heading, .card, .content-box, .efficiency-box"
);

const scrollObserver = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add("show");
            scrollObserver.unobserve(entry.target);
        }
    });
}, { threshold: 0.15 });

animatedItems.forEach(item => {
    item.classList.add("animate-smooth");
    scrollObserver.observe(item);
});


/* =========================================================
   2. CODE POPUP LOGIC (Diagnostic Version)
========================================================= */

function openModal(btn) {
    // 1. Get Modal
    const modal = document.getElementById("codeModal");
    if (!modal) {
        alert("CRITICAL ERROR: Modal HTML missing.");
        return;
    }

    // 2. Open & Reset
    modal.classList.add("open"); 
    
    // Set initial status text
    const setStatus = (id, msg) => {
        const el = document.querySelector(`#${id} code`);
        if(el) el.textContent = msg;
    };

    setStatus("cpp", "🔍 Scanning for C++ file...");
    setStatus("input", "🔍 Scanning for Input...");
    setStatus("output", "🔍 Scanning for Output...");

    // 3. Reset Tabs
    switchTab(null, 'cpp');

    // 4. Find Data Container
    let container = btn.nextElementSibling;
    while (container && !container.classList.contains("code-data")) {
        container = container.nextElementSibling;
    }

    if (!container) {
        setStatus("cpp", "❌ ERROR: No <div class='code-data'> found next to button.");
        return;
    }

    // 5. Load Content Function
    const loadContent = (type, elementId) => {
        const div = container.querySelector(`.data-${type}`);
        const codeBox = document.querySelector(`#${elementId} code`);

        if (!div) {
            codeBox.textContent = "// No " + type + " data provided.";
            return;
        }

        // Case A: Fetch from File
        if (div.dataset.src) {
            const filePath = div.dataset.src;
            codeBox.textContent = "⏳ Loading file: " + filePath + " ...";

            // Add timestamp to force fresh load (bypasses cache)
            fetch(filePath + "?t=" + new Date().getTime())
                .then(response => {
                    if (!response.ok) throw new Error(`File not found (Status: ${response.status})`);
                    return response.text();
                })
                .then(text => {
                    // SUCCESS: Display the code
                    codeBox.textContent = text; 
                })
                .catch(err => {
                    // FAILURE: Show error message in the box
                    codeBox.textContent = "❌ ERROR: Could not load file.\n" +
                                          "👉 Path: " + filePath + "\n" +
                                          "👉 Details: " + err.message + "\n" +
                                          "👉 Check: Does '" + filePath + "' exist in your 'codes' folder?";
                });
        } 
        // Case B: Inline HTML
        else if (div.innerHTML.trim().length > 0) {
            codeBox.textContent = div.innerHTML.trim(); // Use textContent to show tags like <iostream> safely
        } 
        else {
            codeBox.textContent = "// Data block is empty.";
        }
    };

    // 6. Trigger Loads
    loadContent("cpp", "cpp");
    loadContent("input", "input");
    loadContent("output", "output");
}


function closeCode() {
    const modal = document.getElementById("codeModal");
    if (modal) modal.classList.remove("open");
}

window.onclick = function(event) {
    const modal = document.getElementById("codeModal");
    if (event.target === modal) closeCode();
};

function switchTab(evt, tabName) {
    document.querySelectorAll(".code-content").forEach(c => c.classList.remove("active-content"));
    document.querySelectorAll(".tab-btn").forEach(b => b.classList.remove("active"));
    
    const activeContent = document.getElementById(tabName);
    if (activeContent) activeContent.classList.add("active-content");

    if (evt) {
        evt.currentTarget.classList.add("active");
    } else {
        const btn = document.querySelector(`.tab-btn[onclick*="${tabName}"]`);
        if (btn) btn.classList.add("active");
    }
}
