let data = null;
let trajectories = {};
let planetVisibility = {};   // NEW : visibilité des planètes
let t = 0;
let scaleFactor;
let planetPositions = {};
let slider;


function setup() {
    let canvas = createCanvas(700, 700);
    canvas.parent("canvasContainer");
    background(0);
    slider = createSlider(0, 2, 0, 0.01);
    slider.parent("canvasContainer");
    scaleFactor = 1e-9*slider.value(); // Ajustez selon vos données


    document.getElementById("fileInput").addEventListener("change", loadJSONFile);
}

function draw() {
    background(0);

    scaleFactor = 1e-9*slider.value();
    console.log("Scale factor:", scaleFactor);
    // Soleil
    fill(255, 200, 0);
    noStroke();
    ellipse(width/2, height/2, 20, 20);

    if (!data) return;

    for (let planetName in trajectories) {

        // Si la case n'est pas cochée → on saute
        if (!planetVisibility[planetName]) continue;

        let traj = trajectories[planetName];

        // Orbite
        stroke(150);
        noFill();
        beginShape();
        for (let p of traj) {
            let x = width/2 + p[0][0] * scaleFactor;
            let y = height/2 + p[0][1] * scaleFactor;
            vertex(x, y);
        }
        endShape();

        // Animation
        let index = t % traj.length;
        let pos = traj[index][0];

        fill(0, 150, 255);
        noStroke();
        ellipse(width/2 + pos[0] * scaleFactor,
                height/2 + pos[1] * scaleFactor,
                10, 10);
        let px = width/2 + pos[0] * scaleFactor;
        let py = height/2 + pos[1] * scaleFactor;

        planetPositions[planetName] = { x: px, y: py };


        fill(255);
        text(planetName,
             width/2 + pos[0] * scaleFactor + 12,
             height/2 + pos[1] * scaleFactor);
    }

    t++;
}

function loadJSONFile(event) {
    let file = event.target.files[0];
    let reader = new FileReader();

    reader.onload = function(e) {
        data = JSON.parse(e.target.result);
        trajectories = data;

        // Création dynamique des cases à cocher
        createCheckboxes();

        console.log("Trajectoires chargées :", trajectories);
    };

    reader.readAsText(file);
}

function createCheckboxes() {
    let container = document.getElementById("planetCheckboxes");
    container.innerHTML = ""; // reset

    for (let planetName in trajectories) {

        // Par défaut : visible
        planetVisibility[planetName] = true;

        let label = document.createElement("label");
        let checkbox = document.createElement("input");

        checkbox.type = "checkbox";
        checkbox.checked = true;
        checkbox.dataset.planet = planetName;

        checkbox.addEventListener("change", function() {
            planetVisibility[this.dataset.planet] = this.checked;
        });

        label.appendChild(checkbox);
        label.appendChild(document.createTextNode(" " + planetName));

        container.appendChild(label);
    }
}

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
        "Mercure": "Première planète du système solaire. Très chaude.",
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
let planetNames = ["Mercure", "Vénus", "Terre", "Mars", "Jupiter", "Saturne", "Uranus", "Neptune"];

function createAccordion() {
    let container = document.getElementById("accordionContainer");

    planetNames.forEach(name => {
        container.innerHTML += `
            <div class="accordion-item">
                <button class="accordion-header">${name}</button>
                <div class="accordion-content">
                    <button onclick="selectMethod('${name}', 'Euler')">Trajectoire Euler</button>
                    <button onclick="selectMethod('${name}', 'EK')">Trajectoire EK</button>
                </div>
            </div>
        `;
    });

    activateAccordion();
}

function activateAccordion() {
    let headers = document.querySelectorAll(".accordion-header");

    headers.forEach(header => {
        header.addEventListener("click", function() {
            let content = this.nextElementSibling;

            if (content.style.maxHeight) {
                content.style.maxHeight = null;
            } else {
                content.style.maxHeight = content.scrollHeight + "px";
            }
        });
    });
}

createAccordion();




