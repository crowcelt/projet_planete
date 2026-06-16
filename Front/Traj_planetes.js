/* ============================
   VARIABLES GLOBALES
============================ */

let trajectories = {};
let trajectoryVisibility = {};
let planetPositions = {};
let dtByPlanet = {};
let methodByPlanet = {};
let t = 0;
let scaleFactor = 1;
let slider;
let starBackground;

// Facteur réglable pour la taille des planètes 
let planetSizeFactor = 0.001;

// Empêche la fermeture immédiate du popup
let popupJustOpened = false;

// Rayons réels des planètes (km)
const realPlanetRadius = {
    "Mercure": 2440,
    "Vénus": 6052,
    "Terre": 6371,
    "Mars": 3389,
    "Jupiter": 69911,
    "Saturne": 58232,
    "Uranus": 25362,
    "Neptune": 24622
};

/* ============================
   PRELOAD
============================ */

function preload() {
    starBackground = loadImage("etoiles.jpg");
}

/* ============================
   SETUP
============================ */

function setup() {
    let canvas = createCanvas(700, 700);
    canvas.parent("canvasContainer");

    slider = select("#zoomSlider");

    document.getElementById("fileInput")
        .addEventListener("change", loadJSONFile);
}

/* ============================
   DRAW
============================ */

const planetcolor = {
    "Mercure": "#4b4141",
    "Vénus": "#e5c27a",
    "Terre": "#6b93d6",
    "Mars": "#c1440e",
    "Jupiter": "#d8ca9d",
    "Saturne": "#f0e68c",
    "Uranus": "#afeeee",
    "Neptune": "#4169e1"
};

function draw() {
    background(0);

    scaleFactor = 1e-9 * slider.value();

    /* Fond étoilé */
    if (starBackground) {
        push();
        imageMode(CORNER);
        image(starBackground, 0, 0, width, height);
        pop();
    }

    /* Soleil */
    fill(255, 200, 0);
    noStroke();
    ellipse(width / 2, height / 2, 20, 20);

    if (Object.keys(trajectories).length === 0) return;

    for (let planetName in trajectories) {
        if (!trajectoryVisibility[planetName]) continue;

        let trajData = trajectories[planetName];

        if (trajectoryVisibility[planetName].Euler && trajData.Euler)
            drawTrajectory(trajData.Euler, planetcolor[planetName], planetName);

        if (trajectoryVisibility[planetName].RK2 && trajData.RK2)
            drawTrajectory(trajData.RK2, planetcolor[planetName], planetName);
    }

    t++;
}

/* ============================
   TRAJECTOIRE + ANIMATION
============================ */

function drawTrajectory(traj, color, planetName) {
    if (!traj || !traj.length) return;

    stroke(color);
    noFill();
    beginShape();

    for (let p of traj) {
        let x = width / 2 + p[0][0] * scaleFactor;
        let y = height / 2 + p[0][1] * scaleFactor;
        vertex(x, y);
    }

    endShape();

    /* Animation */
    const dt = dtByPlanet[planetName] || 3600;
    const baseDt = 14400;
    const step = Math.max(1, Math.floor(baseDt / dt));

    let index = t % traj.length;
    let pos = traj[index][0];

    let px = width / 2 + pos[0] * scaleFactor;
    let py = height / 2 + pos[1] * scaleFactor;

    /* Taille réaliste */
    let radiusKm = realPlanetRadius[planetName] || 3000;
    let displaySize = radiusKm * planetSizeFactor;

    fill(color);
    noStroke();
    ellipse(px, py, displaySize, displaySize);

    planetPositions[planetName] = { x: px, y: py };

    fill(255);
    text(planetName, px + 12, py);
}

/* ============================
   NORMALISATION JSON
============================ */

function normalizePlanetName(name) {
    name = name.toLowerCase();

    const map = {
        "mercury": "Mercure",
        "mercure": "Mercure",
        "venus": "Vénus",
        "vénus": "Vénus",
        "earth": "Terre",
        "terre": "Terre",
        "mars": "Mars",
        "jupiter": "Jupiter",
        "saturn": "Saturne",
        "saturne": "Saturne",
        "uranus": "Uranus",
        "neptune": "Neptune"
    };

    return map[name] || name;
}

function normalizeMethodName(name) {
    name = name.toLowerCase();

    if (name.includes("euler")) return "Euler";
    if (name.includes("rk2")) return "RK2";
    if (name.includes("runge")) return "RK2";
    if (name.includes("rk")) return "RK2";

    return name;
}

function extractDtFromKey(key) {
    const match = key.match(/dt(\d+)/);
    return match ? Number(match[1]) : 3600;
}

function normalizeTrajectory(data) {
    let result = [];

    for (let entry of data) {
        if (Array.isArray(entry) && Array.isArray(entry[0])) {
            result.push(entry);
        }
        else if (Array.isArray(entry) && typeof entry[0] === "number") {
            result.push([entry]);
        }
        else if (entry.x !== undefined && entry.y !== undefined) {
            result.push([[entry.x, entry.y]]);
        }
    }

    return result;
}

/* ============================
   CHARGEMENT JSON
============================ */

function loadJSONFile(event) {
    let file = event.target.files[0];
    if (!file) return;

    let reader = new FileReader();

    reader.onload = function (e) {
        try {
            const raw = JSON.parse(e.target.result);

            for (let key in raw) {
                let cleanKey = key.replace(/[_ ]/g, "-");

                let parts = cleanKey.split("-");
                if (parts.length < 2) continue;

                let planetKey = normalizePlanetName(parts[0]);
                let methodKey = normalizeMethodName(parts[1]);
                let dt = extractDtFromKey(cleanKey);

                if (!trajectories[planetKey])
                    trajectories[planetKey] = {};

                trajectories[planetKey][methodKey] =
                    normalizeTrajectory(raw[key]);

                dtByPlanet[planetKey] = dt;
                methodByPlanet[planetKey] = methodKey;
            }

            createAccordion();
            activateCheckboxes();

        } catch (err) {
            console.error("Erreur JSON :", err);
        }
    };

    reader.readAsText(file);
}

/* ============================
   ACCORDÉON
============================ */

function createAccordion() {
    let container = document.getElementById("accordionContainer");
    container.innerHTML = "";

    for (let planetName in trajectories) {
        container.innerHTML += `
            <div class="accordion-item">
                <button class="accordion-header">${planetName}</button>
                <div class="accordion-content">
                    <label>
                        <input type="checkbox" class="trajCheck"
                               data-planet="${planetName}"
                               data-method="Euler"
                               ${trajectories[planetName].Euler ? "checked" : ""}>
                        Euler
                    </label>

                    <label>
                        <input type="checkbox" class="trajCheck"
                               data-planet="${planetName}"
                               data-method="RK2"
                               ${trajectories[planetName].RK2 ? "checked" : ""}>
                        RK2
                    </label>
                </div>
            </div>
        `;

        trajectoryVisibility[planetName] = {
            Euler: !!trajectories[planetName].Euler,
            RK2: !!trajectories[planetName].RK2
        };
    }

    activateAccordion();
}

function activateAccordion() {
    let headers = document.querySelectorAll(".accordion-header");

    headers.forEach(header => {
        header.addEventListener("click", function () {
            let content = this.nextElementSibling;

            if (content.style.maxHeight)
                content.style.maxHeight = null;
            else
                content.style.maxHeight = content.scrollHeight + "px";
        });
    });
}

function activateCheckboxes() {
    const checks = document.querySelectorAll(".trajCheck");

    checks.forEach(chk => {
        chk.addEventListener("change", () => {
            const planet = chk.dataset.planet;
            const method = chk.dataset.method;

            trajectoryVisibility[planet][method] = chk.checked;
        });
    });
}

/* ============================
   POPUP → PANNEAU LATÉRAL DROIT
============================ */

function isMouseOnPlanet(px, py, radius = 10) {
    let dx = mouseX - px;
    let dy = mouseY - py;
    return dx * dx + dy * dy <= radius * radius;
}

function mousePressed() {
    for (let planetName in planetPositions) {
        let p = planetPositions[planetName];

        if (isMouseOnPlanet(p.x, p.y, 10)) {
            openPlanetPopup(planetName);
        }
    }
}

function openPlanetPopup(name) {

    popupJustOpened = true; // Empêche la fermeture immédiate

    setTimeout(() => popupJustOpened = false, 100);

    document.getElementById("popupTitle").textContent = name;

    document.getElementById("popupDetails").innerHTML =
        planetDescriptions[name] || "Aucune information disponible.";

    const img = document.getElementById("popupImage");
    img.src = planetImages[name] || "";
    img.style.display = planetImages[name] ? "block" : "none";

    const moreLink = document.getElementById("popupMore");
    moreLink.href = planetLinks[name] || "#";

    document.getElementById("planetInfoPanel").classList.add("open");
}

/* Fermeture via la croix */
document.getElementById("closePopup").onclick = function () {
    document.getElementById("planetInfoPanel").classList.remove("open");
};

/* Fermeture via Échap */
document.addEventListener("keydown", function (event) {
    if (event.key === "Escape") {
        document.getElementById("planetInfoPanel").classList.remove("open");
    }
});

/* Fermeture en cliquant dans le vide */
document.addEventListener("click", function (event) {

    const panel = document.getElementById("planetInfoPanel");

    if (popupJustOpened) return;

    if (panel.classList.contains("open") && !panel.contains(event.target)) {
        panel.classList.remove("open");
    }
});

/* ============================
   DESCRIPTIONS
============================ */

const planetDescriptions = {
    /* Mets ici la version détaillée que je t’ai envoyée */
};

const planetImages = {
    "Mercure": "mercure.jpg",
    "Vénus": "venus.jpg",
    "Terre": "terre.jpg",
    "Mars": "mars.jpg",
    "Jupiter": "jupiter.jpg",
    "Saturne": "saturne.jpg",
    "Uranus": "uranus.jpg",
    "Neptune": "neptune.jpg"
};

const planetLinks = {
    "Mercure": "https://cnes.fr/dossiers/planete-mercure",
    "Vénus": "https://cnes.fr/dossiers/planete-venus",
    "Terre": "https://cnes.fr/dossiers/planete-terre",
    "Mars": "https://cnes.fr/dossiers/planete-mars",
    "Jupiter": "https://cnes.fr/dossiers/planete-jupiter",
    "Saturne": "https://cnes.fr/dossiers/planete-saturne",
    "Uranus": "https://cnes.fr/dossiers/planete-uranus",
    "Neptune": "https://cnes.fr/dossiers/planete-neptune"
};
