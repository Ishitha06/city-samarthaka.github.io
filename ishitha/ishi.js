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
            scrollObserver.unobserve(entry.target);
        }
    });
}, { threshold: 0.15 });

animatedItems.forEach(item => {
    item.classList.add("animate-smooth");
    scrollObserver.observe(item);
});


/* =========================================================
   3. CODE POPUP LOGIC (Fixed & Robust)
========================================================= */

function openModal(btnElement) {
    const modal = document.getElementById('codeModal');
    if (!modal) return; // Safety check
    const codeWindow = modal.querySelector('.code-window');

    // --- 1. ROBUST DATA FETCHING ---
    // We look for the next sibling that is actually a '.code-data' div.
    // This skips <br> tags, text nodes, or comments that might be in between.
    let dataDiv = btnElement.nextElementSibling;
    while (dataDiv && !dataDiv.classList.contains('code-data')) {
        dataDiv = dataDiv.nextElementSibling;
    }

    // Only try to extract data if we actually found the div
    if (dataDiv) {
        try {
            const cppCode = dataDiv.querySelector('.data-cpp').innerHTML;
            const inputCode = dataDiv.querySelector('.data-input').innerHTML;
            const outputCode = dataDiv.querySelector('.data-output').innerHTML;

            // Inject into Popup
            document.querySelector('#cpp code').innerHTML = cppCode;
            document.querySelector('#input code').innerHTML = inputCode;
            document.querySelector('#output code').innerHTML = outputCode;
        } catch (err) {
            console.error("Error reading code data:", err);
            document.querySelector('#cpp code').innerText = "// Error: Code structure missing.";
        }
    } else {
        // Fallback if no data div found
        console.warn("No .code-data div found next to button");
        document.querySelector('#cpp code').innerText = "// No code data provided for this section.";
    }

    // --- 2. POSITIONING LOGIC ---
    modal.style.height = document.documentElement.scrollHeight + 'px';
    modal.style.display = 'block';

    const rect = btnElement.getBoundingClientRect();
    const scrollTop = window.scrollY || document.documentElement.scrollTop;
    
    const popupWidth = 700; 
    const popupHeight = 450;
    const gap = 15;

    // Calculate Top: Position it ABOVE the button
    let topPos = rect.top + scrollTop - popupHeight - gap;

    // Calculate Left: Center it horizontally relative to the button
    let leftPos = rect.left + (rect.width / 2) - (popupWidth / 2);

    // Safety Checks (Keep it on screen)
    if (leftPos < 10) leftPos = 10;
    if (leftPos + popupWidth > document.documentElement.clientWidth) {
        leftPos = document.documentElement.clientWidth - popupWidth - 10;
    }

    // Check Top Edge: If button is too high, show popup BELOW button
    if (topPos < scrollTop) {
        topPos = rect.bottom + scrollTop + gap;
    }

    codeWindow.style.top = topPos + 'px';
    codeWindow.style.left = leftPos + 'px';

    // Reset tabs to first one
    switchTab(null, 'cpp');

    // Animate in
    setTimeout(() => {
        codeWindow.classList.add('show');
    }, 10);
}

function closeCode() {
    const modal = document.getElementById('codeModal');
    const codeWindow = modal.querySelector('.code-window');

    codeWindow.classList.remove('show');

    setTimeout(() => {
        modal.style.display = 'none';
    }, 200);
}

window.onclick = function(event) {
    const modal = document.getElementById('codeModal');
    if (event.target == modal) {
        closeCode();
    }
}

// Fixed Switch Tab Logic (Handles Event Correctly)
function switchTab(evt, tabName) {
    // Hide all contents
    document.querySelectorAll('.code-content').forEach(content => {
        content.classList.remove('active-content');
    });

    // Deactivate all buttons
    document.querySelectorAll('.tab-btn').forEach(btn => {
        btn.classList.remove('active');
    });

    // Show selected content
    const activeContent = document.getElementById(tabName);
    if(activeContent) activeContent.classList.add('active-content');
    
    // Activate button
    // If event is passed (click), use it. If null (reset), find button manually.
    if (evt) {
        evt.currentTarget.classList.add('active');
    } else {
        // Default to first button if no event (reset scenario)
        const defaultBtn = document.querySelector(`.tab-btn[onclick*="'${tabName}'"]`);
        if(defaultBtn) defaultBtn.classList.add('active');
    }
}