/* ============================
   VARIABLES GLOBALES
============================ */

let trajectories = {};          // Données JSON regroupées par planète
let trajectoryVisibility = {};  // { Mercure: {Euler:true, RK2:true}, ... }
let planetPositions = {};       // Pour le popup
let t = 0;                      // Animation
let scaleFactor = 1;
let slider;                     // Slider HTML
let starBackground;             // Image étoilée


/* ============================
   PRELOAD : charger l'image étoilée
============================ */

function preload() {
    starBackground = loadImage("etoiles.jpg");
}


/* ============================
   SETUP P5.JS
============================ */

function setup() {
    let canvas = createCanvas(700, 700);
    canvas.parent("canvasContainer");

    slider = select("#zoomSlider");

    // MULTI‑JSON : on peut charger plusieurs fichiers sans écraser
    document.getElementById("fileInput")
        .addEventListener("change", loadJSONFile);
}


/* ============================
   DRAW P5.JS
============================ */

function draw() {
    background(0);

    scaleFactor = 1e-9 * slider.value();

    /* === FOND ÉTOILÉ QUI ZOOME === */
    push();
    translate(width/2, height/2);
    scale(scaleFactor * 0.15);
    imageMode(CENTER);
    image(starBackground, 0, 0, starBackground.width, starBackground.height);
    pop();

    // Soleil
    fill(255, 200, 0);
    noStroke();
    ellipse(width/2, height/2, 20, 20);

    if (Object.keys(trajectories).length === 0) return;

    for (let planetName in trajectories) {

        if (!trajectoryVisibility[planetName]) continue;

        let trajData = trajectories[planetName];

        if (trajectoryVisibility[planetName].Euler && trajData.Euler)
            drawTrajectory(trajData.Euler, "cyan", planetName);

        if (trajectoryVisibility[planetName].RK2 && trajData.RK2)
            drawTrajectory(trajData.RK2, "magenta", planetName);
    }

    t++;
}


/* ============================
   FONCTION D'AFFICHAGE TRAJECTOIRE
============================ */

function drawTrajectory(traj, color, planetName) {

    stroke(color);
    noFill();
    beginShape();
    for (let p of traj) {
        let x = width/2 + p[0][0] * scaleFactor;
        let y = width/2 + p[0][1] * scaleFactor;
        vertex(x, y);
    }
    endShape();

    let index = t % traj.length;
    let pos = traj[index][0];

    let px = width/2 + pos[0] * scaleFactor;
    let py = height/2 + pos[1] * scaleFactor;

    fill(color);
    ellipse(px, py, 10, 10);

    planetPositions[planetName] = { x: px, y: py };

    fill(255);
    text(planetName, px + 12, py);
}


/* ============================
   IMPORT JSON + FUSION MULTI‑JSON
============================ */

function loadJSONFile(event) {
    let file = event.target.files[0];
    if (!file) return;

    let reader = new FileReader();

    reader.onload = function(e) {
        try {
            const raw = JSON.parse(e.target.result);

            const planetNameMap = {
                "mercury": "Mercure",
                "venus": "Vénus",
                "earth": "Terre",
                "mars": "Mars",
                "jupiter": "Jupiter",
                "saturn": "Saturne",
                "uranus": "Uranus",
                "neptune": "Neptune"
            };

            const methodNameMap = {
                "euler": "Euler",
                "rk2": "RK2"
            };

            // FUSION : on n'efface PAS trajectories
            for (let key in raw) {
                const parts = key.split("-");
                if (parts.length !== 2) continue;

                const planetKey = parts[0].toLowerCase();
                const methodKey = parts[1].toLowerCase();

                const planetName = planetNameMap[planetKey] || planetKey;
                const methodName = methodNameMap[methodKey] || methodKey;

                if (!trajectories[planetName])
                    trajectories[planetName] = {};

                trajectories[planetName][methodName] = raw[key];
            }

            console.log("Trajectoires fusionnées :", trajectories);

            createAccordion();
            activateCheckboxes();

        } catch (err) {
            console.error("Erreur JSON :", err);
        }
    };

    reader.readAsText(file);
}


/* ============================
   ACCORDÉON + CASES À COCHER
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
                               data-method="Euler" ${trajectories[planetName].Euler ? "checked" : ""}>
                        Trajectoire Euler
                    </label>

                    <label>
                        <input type="checkbox" class="trajCheck"
                               data-planet="${planetName}"
                               data-method="RK2" ${trajectories[planetName].RK2 ? "checked" : ""}>
                        Trajectoire RK2
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
        header.addEventListener("click", function() {
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
   POPUP PLANÈTES
============================ */

function isMouseOnPlanet(px, py, radius = 10) {
    let dx = mouseX - px;
    let dy = mouseY - py;
    return dx*dx + dy*dy <= radius*radius;
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
    document.getElementById("popupTitle").textContent = name;

    let details = {
        "Mercure": "Première planète du système solaire.",
        "Vénus": "Atmosphère dense, effet de serre extrême.",
        "Terre": "Notre planète bleue.",
        "Mars": "La planète rouge.",
        "Jupiter": "La plus grande planète.",
        "Saturne": "Connue pour ses anneaux.",
        "Uranus": "Rotation inclinée.",
        "Neptune": "Planète bleue glacée."
    };

    document.getElementById("popupDetails").textContent =
        details[name] || "Aucune information disponible.";

    document.getElementById("planetInfoPopup").style.display = "flex";
}

document.getElementById("closePopup").onclick = function() {
    document.getElementById("planetInfoPopup").style.display = "none";
};

window.onclick = function(event) {
    let popup = document.getElementById("planetInfoPopup");
    if (event.target === popup) {
        popup.style.display = "none";
    }
};

document.addEventListener("keydown", function(event) {
    if (event.key === "Escape") {
        document.getElementById("planetInfoPopup").style.display = "none";
    }
});
