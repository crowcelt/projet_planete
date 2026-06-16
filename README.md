# Projet Kepler — Simulation du Système Solaire
**CIR 1 — ISEN Yncréa Ouest — 2025-2026**

---

## Description

Ce projet simule les trajectoires des 8 planètes du système solaire par résolution numérique d'équations différentielles. Le calcul est effectué en C et les résultats sont exportés en JSON pour être visualisés dans une interface web en JavaScript avec Traj_planetes.js.

---

## Structure du projet

```
projet_c/
├── main.c                  # Point d'entrée du programme
├── Makefile                # Compilation
├── README.md               # Ce fichier
│
├── header/                 # Fichiers d'en-tête (.h)
│   ├── vector.h            # Struct Vector et opérations mathématiques
│   ├── trajectory.h        # Struct Point et Struct Trajectory
│   ├── planet.h            # Struct Planet et constantes physiques
│   ├── solvers.h           # Méthodes d'intégration numérique
│   ├── energy.h            # Conservation de l'énergie
│   └── export.h            # Export JSON
│
├── c_files/                # Fichiers source (.c)
│   ├── vector.c
│   ├── trajectory.c
│   ├── planet.c
│   ├── solvers.c
│   ├── energy.c
│   └── export.c
│
├── objects/                # Fichiers objets (.o) générés à la compilation
│
└── output/                 # Fichiers JSON générés à l'exécution
    ├── solar_system_euler.json
    ├── solar_system_euler_asym.json
    └── solar_system_rk2.json
```

---

## Exécution

Le programme génère automatiquement 3 fichiers dans le dossier `output/` :

| Fichier | Méthode |
|---------|---------|
| `solar_system_euler.json` | Euler simple |
| `solar_system_euler_asym.json` | Euler asymétrique |
| `solar_system_rk2.json` | Runge-Kutta ordre 2 |

---

## Méthodes d'intégration

### Euler simple
Méthode de base. Calcule l'accélération depuis la position actuelle, puis met à jour position et vitesse. **Diverge rapidement** — l'énergie totale augmente à chaque pas.

### Euler asymétrique (symplectique)
Même principe mais la position est mise à jour en premier, puis l'accélération est recalculée depuis cette nouvelle position avant de mettre à jour la vitesse. **Conserve bien mieux l'énergie.**

### Runge-Kutta ordre 2 (RK2)
Calcule un point intermédiaire au milieu du pas de temps pour affiner l'approximation. **Meilleure précision**, erreur d'ordre 2 en Δt.

---

## Format d'échange JSON

Le format est imposé par le sujet et commun à tous les groupes. Chaque planète est identifiée par son nom suivi de la méthode utilisée et du pas de temps.

```json
{
  "earth-RK2-dt43200": [
    [[1.470000e+11, 0.000000e+00, 0.000000e+00],
     [0.000000e+00, 3.028629e+04, 0.000000e+00], 0.000000e+00],
  ]
}
```

Chaque point contient :
- `[rx, ry, rz]` — position en mètres
- `[vx, vy, vz]` — vitesse en m/s
- `t` — temps réel en secondes

---

## Pas de temps par planète

Chaque planète utilise un pas de temps adapté à sa période orbitale pour éviter la divergence tout en limitant la taille des fichiers JSON.

| Planète | Pas de temps | Raison |
|---------|-------------|--------|
| Mercure | 4h | Orbite rapide, proche du Soleil |
| Vénus | 12h | |
| Terre | 12h | |
| Mars | 1 jour | |
| Jupiter | 5 jours | |
| Saturne | 10 jours | |
| Uranus | 30 jours | |
| Neptune | 60 jours | Orbite très lente |

Chaque planète est simulée sur environ 2 orbites complètes.

---

## Conservation de l'énergie

Le programme vérifie que l'énergie totale du système reste constante au fil du temps :

```
E = Ec + Ep = constante
Ec = ½ · m · ||v||²          (énergie cinétique)
Ep = −G · m · mS / ||r||     (énergie potentielle)
```

## Constantes physiques utilisées

| Constante | Valeur |
|-----------|--------|
| G | 6.67408 × 10⁻¹¹ N·m²·kg⁻² |
| Masse du Soleil | 1.989 × 10³⁰ kg |
| Masse de la Terre | 5.972 × 10²⁴ kg |
| Périhélie Terre | 1.470 × 10¹¹ m |

Toutes les constantes sont définies dans `header/planet.h`.

## Auteurs

- **Partie C** (calcul des trajectoires) — *Raphael Corre*
- **Partie Front-end** (visualisation p5.js) — *Mickael Le Gall Corre*