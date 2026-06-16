/* ============================
   VARIABLES GLOBALES
============================ */

let trajectories = {};
let trajectoryVisibility = {};
let planetPositions = {};
let dtByPlanet = {};
let methodByPlanet = {};
let tByPlanet = {};
let scaleFactor = 1;
let slider;
let speedSlider;
let starBackground;
let simulatedSeconds = 0;

let planetSizeFactor = 0.001;
let popupJustOpened = false;

/* Centre du système solaire */
let systemCenterX;
let systemCenterY;

/* Rayons réels des planètes (km) */
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
    let canvas = createCanvas(windowWidth, windowHeight);
    canvas.parent("canvasContainer");

    systemCenterX = windowWidth * 0.55;
    systemCenterY = windowHeight * 0.5;

    canvas.elt.addEventListener("click", (e) => e.stopPropagation());

    slider = select("#zoomSlider");
    speedSlider = select("#speedSlider");

    document.getElementById("fileInput")
        .addEventListener("change", loadJSONFile);
}

/* Redimensionnement automatique */
function windowResized() {
    resizeCanvas(windowWidth, windowHeight);
    systemCenterX = windowWidth * 0.55;
    systemCenterY = windowHeight * 0.5;
}

/* ============================
   DRAW
============================ */

const planetcolor = {
    "Mercure": "#b1b1b1",
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

    if (starBackground) {
        image(starBackground, 0, 0, width, height);
    }

    /* Soleil */
    fill(255, 200, 0);
    noStroke();
    ellipse(systemCenterX, systemCenterY, 20, 20);

    // ⭐ AJOUT — compteur de jours AVANT le return
    let speedFactor = speedSlider.value() / 50;
    simulatedSeconds += (deltaTime / 1000) * speedFactor;
    let simulatedDays = Math.floor(simulatedSeconds / 86400);
    document.getElementById("dayCounter").textContent =
        "Jours écoulés : " + simulatedDays;
    // ⭐ FIN AJOUT

    if (Object.keys(trajectories).length === 0) return;

    for (let planetName in trajectories) {
        if (!trajectoryVisibility[planetName]) continue;

        let trajData = trajectories[planetName];

        if (trajectoryVisibility[planetName].Euler && trajData.Euler)
            drawTrajectory(trajData.Euler, planetcolor[planetName], planetName);

        if (trajectoryVisibility[planetName].RK2 && trajData.RK2)
            drawTrajectory(trajData.RK2, planetcolor[planetName], planetName);
    }
}

/* ============================
   TRAJECTOIRE + ANIMATION RÉALISTE
============================ */

function drawTrajectory(traj, color, planetName) {
    if (!traj || !traj.length) return;

    stroke(color);
    noFill();
    beginShape();

    for (let p of traj) {
        let x = systemCenterX + p[0][0] * scaleFactor;
        let y = systemCenterY + p[0][1] * scaleFactor;
        vertex(x, y);
    }

    endShape();

    const dt = dtByPlanet[planetName] || 3600;
    const accelerationFactor = 50000;

    if (!tByPlanet[planetName]) tByPlanet[planetName] = 0;

    let speedFactor = speedSlider.value() / 50;

    tByPlanet[planetName] += (deltaTime / 1000) * (accelerationFactor / dt) * speedFactor;

    let index = Math.floor(tByPlanet[planetName]) % traj.length;
    let pos = traj[index][0];

    let px = systemCenterX + pos[0] * scaleFactor;
    let py = systemCenterY + pos[1] * scaleFactor;

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
                let cleanKey = key.replace(/[\/ _]/g, "-").toLowerCase();

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

                if (!tByPlanet[planetKey]) tByPlanet[planetKey] = 0;
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
//pour activer les checkboxes et mettre à jour la visibilité des trajectoires
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
   POPUP LATÉRAL
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

    popupJustOpened = true;
    setTimeout(() => popupJustOpened = false, 150);

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
//pour fermer le popup avec la croix
document.getElementById("closePopup").onclick = function () {
    document.getElementById("planetInfoPanel").classList.remove("open");
};
//pour fermer le popup avec echap
document.addEventListener("keydown", function (event) {
    if (event.key === "Escape") {
        document.getElementById("planetInfoPanel").classList.remove("open");
    }
});

//DESCRIPTIONS

const planetDescriptions = {

    "Mercure": `
        <h3>Caractéristiques générales</h3>
        Mercure est la plus petite planète du Système solaire et la plus proche du Soleil.
        Elle possède un diamètre d’environ <b>4 880 km</b> et une densité très élevée.

        <h3>Surface et géologie</h3>
        Sa surface est recouverte de cratères, semblable à celle de la Lune.
        Elle ne possède presque pas d’atmosphère, ce qui laisse le sol exposé aux impacts
        et aux variations extrêmes de température.

        <h3>Température</h3>
        - Côté jour : jusqu’à <b>+430°C</b><br>
        - Côté nuit : jusqu’à <b>-180°C</b>

        <h3>Particularités</h3>
        - Rotation très lente : un jour mercurien dure 176 jours terrestres.<br>
        - Noyau métallique surdimensionné occupant plus de 60% du volume total.
    `,

    "Vénus": `
        <h3>Caractéristiques générales</h3>
        Vénus est presque aussi grande que la Terre (diamètre : <b>12 104 km</b>), 
        mais son environnement est radicalement différent.

        <h3>Atmosphère</h3>
        Son atmosphère est composée à <b>96% de CO₂</b> et recouverte de nuages d’acide sulfurique.
        La pression au sol est équivalente à celle ressentie à 900 m sous l’eau.

        <h3>Température</h3>
        - Température moyenne : <b>≈ 470°C</b><br>
        L’effet de serre y est le plus intense du Système solaire.

        <h3>Particularités</h3>
        - Rotation rétrograde (elle tourne à l’envers).<br>
        - Journée plus longue que son année.<br>
        - Aucune lune connue.
    `,

    "Terre": `
        <h3>Caractéristiques générales</h3>
        La Terre est la seule planète connue abritant la vie.
        Diamètre : <b>12 742 km</b>. Atmosphère riche en azote et oxygène.

        <h3>Environnement</h3>
        - Présence d’eau liquide en abondance.<br>
        - Températures modérées grâce à l’effet de serre naturel.<br>
        - Champ magnétique puissant protégeant des particules solaires.

        <h3>Particularités</h3>
        - Unique satellite : <b>la Lune</b>.<br>
        - Plaques tectoniques actives.<br>
        - Biosphère extrêmement diversifiée.
    `,

    "Mars": `
        <h3>Caractéristiques générales</h3>
        Mars est une planète rocheuse de couleur rouge due à l’oxyde de fer.
        Diamètre : <b>6 779 km</b>.

        <h3>Surface</h3>
        - Olympus Mons : plus grand volcan du Système solaire.<br>
        - Valles Marineris : canyon long de 4 000 km.<br>
        - Calottes polaires de glace d’eau et de CO₂.

        <h3>Atmosphère</h3>
        Très fine, composée majoritairement de CO₂.

        <h3>Particularités</h3>
        - Preuves d’anciens cours d’eau.<br>
        - Deux lunes : Phobos et Déimos.<br>
        - Températures : de -140°C à +20°C.
    `,

    "Jupiter": `
        <h3>Caractéristiques générales</h3>
        Jupiter est la plus grande planète du Système solaire (diamètre : <b>139 820 km</b>).
        C’est une géante gazeuse composée d’hydrogène et d’hélium.

        <h3>Atmosphère</h3>
        - Bandes nuageuses colorées.<br>
        - Orages gigantesques.<br>
        - La Grande Tache Rouge : tempête active depuis plus de 300 ans.

        <h3>Satellites</h3>
        Plus de <b>90 lunes</b>, dont les quatre lunes galiléennes :
        Io, Europe, Ganymède et Callisto.

        <h3>Particularités</h3>
        - Champ magnétique extrêmement puissant.<br>
        - Rotation très rapide : un jour dure 10 heures.
    `,

    "Saturne": `
        <h3>Caractéristiques générales</h3>
        Saturne est une géante gazeuse célèbre pour ses anneaux spectaculaires.
        Diamètre : <b>116 460 km</b>.

        <h3>Anneaux</h3>
        Composés de milliards de particules de glace et de roche,
        allant de quelques millimètres à plusieurs mètres.

        <h3>Satellites</h3>
        Plus de <b>80 lunes</b>, dont :
        - Titan : atmosphère dense, lacs d’hydrocarbures.<br>
        - Encelade : geysers d’eau, possible océan souterrain.

        <h3>Particularités</h3>
        - Très faible densité : elle flotterait dans l’eau.<br>
        - Rotation rapide : jour de 10h30.
    `,

    "Uranus": `
        <h3>Caractéristiques générales</h3>
        Uranus est une géante glacée riche en eau, méthane et ammoniaque.
        Diamètre : <b>50 724 km</b>.

        <h3>Atmosphère</h3>
        Couleur bleu-vert due au méthane absorbant la lumière rouge.

        <h3>Inclinaison extrême</h3>
        Son axe de rotation est incliné à <b>98°</b> :
        la planète semble rouler sur son orbite.

        <h3>Particularités</h3>
        - Température minimale : -224°C.<br>
        - Anneaux sombres et fins.<br>
        - Plus de 25 lunes.
    `,

    "Neptune": `
        <h3>Caractéristiques générales</h3>
        Neptune est une géante glacée bleutée, légèrement plus petite qu’Uranus.
        Diamètre : <b>49 244 km</b>.

        <h3>Atmosphère</h3>
        - Vents les plus rapides du Système solaire : jusqu’à 2 100 km/h.<br>
        - Présence de tempêtes sombres similaires à celles de Jupiter.

        <h3>Satellites</h3>
        - Triton : lune majeure, orbite rétrograde, activité géologique.

        <h3>Particularités</h3>
        - Très éloignée du Soleil (4,5 milliards de km).<br>
        - Température moyenne : -220°C.
    `
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
