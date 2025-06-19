// leveleditor.js
document.addEventListener('DOMContentLoaded', function () {
    // Globale Variablen in ein Objekt kapseln
    const LevelEditor = {
        bricks: [],
        typeSelected: "1",
        powerupSelected: "M",

        // Mapping von Brick-Typen zu Bildern
        brickTypeImages: {
            "0": "blank.png",
            "1": "blue.png",
            "2": "yellow.png",
            "3": "cement.png",
            "4": "glass.png",
            "5": "green.png",
            "6": "steel.png",
            "7": "purple.png",
            "8": "white.png",
            "9": "invisible.png",
            "A": "red.png",
            "B": "explosive.png",
            "C": "doom.png",
            "D": "custom.png"
        },

        // Konstanten
        BRICK_COUNT: 598,
        BRICKS_PER_ROW: 26,

        init: function () {
            this.initLevel();
            this.loadData();
            this.setupEventListeners();
        },

        initLevel: function () {
            for (let i = 0; i < this.BRICK_COUNT; i++) {
                this.bricks[i] = ["0", "0"];
            }
        },

        brickGfx: function (brickType) {
            const imageName = this.brickTypeImages[brickType] || "blank.png";
            return `<img src="gfx/${imageName}" border=0 width=32 height=16>`;
        },

        setTypeSelected: function (charType) {
            document.getElementById("html_selectedBrickType").innerHTML =
                "Selected Brick Type: " + this.brickGfx(charType);
            this.typeSelected = charType;
        },

        setPowerupSelected: function () {
            const select = document.getElementById("html_powerupType");
            this.powerupSelected = select.options[select.selectedIndex].value;
            document.getElementById("html_powerupTypeSelected").innerHTML =
                "Powerup Selected: " + this.powerupSelected;
        },

        changeBrick: function (brickNum) {
            if (this.bricks[brickNum][1] === this.typeSelected) {
                this.bricks[brickNum][0] = "0";
                this.bricks[brickNum][1] = "0";
            } else {
                this.bricks[brickNum][0] = this.powerupSelected;
                this.bricks[brickNum][1] = this.typeSelected;
            }

            document.getElementById("brick" + brickNum).innerHTML =
                this.brickGfx(this.bricks[brickNum][1]);
        },

        putBrick: function (brickType, brickNum) {
            return `<a href="#Brick${brickNum}" id="brick${brickNum}" data-brick-num="${brickNum}">
                ${this.brickGfx(brickType)}
            </a>`;
        },

        drawLevel: function () {
            let col = 0;

            // Update leveldata and draw level
            let htmlData = "";
            let levelData = "** Start **\n";

            for (let i = 0; i < this.BRICK_COUNT; i++) {
                if (col === this.BRICKS_PER_ROW) {
                    htmlData += "<br>";
                    levelData += '\n';
                    col = 0;
                }
                col++;

                htmlData += this.putBrick(this.bricks[i][1], i);
                levelData += this.bricks[i][0] + this.bricks[i][1];
            }

            levelData += "\n** Stop **";

            document.getElementById("html_lvl").innerHTML = htmlData;
            document.getElementById("html_leveldata").value = levelData;

            // Nachdem die bricks gerendert wurden, Event-Listener hinzufügen
            this.attachBrickClickListeners();
        },

        loadData: function () {
            try {
                // Read html_lvl and put in bricks[] array.
                let ch = 0;
                let data = document.getElementById("html_leveldata").value;

                // Einfache Validierung
                if (!data.includes("** Start **")) {
                    throw new Error("Ungültiges Levelformat: '** Start **' nicht gefunden");
                }

                data = data.substr(12, data.length);
                data = data.replace(/\n/g, "");

                if (data.length < this.BRICK_COUNT * 2) {
                    throw new Error("Level-Daten zu kurz");
                }

                for (let i = 0; i < this.BRICK_COUNT; i++) {
                    this.bricks[i][0] = data[ch];
                    this.bricks[i][1] = data[ch + 1];
                    ch += 2;
                }

                this.drawLevel();
            } catch (error) {
                alert("Fehler beim Laden der Level-Daten: " + error.message);
                console.error(error);
            }
        },

        attachBrickClickListeners: function () {
            const brickElements = document.querySelectorAll('[id^="brick"]');
            brickElements.forEach(element => {
                element.addEventListener('click', (e) => {
                    e.preventDefault();
                    const brickNum = element.getAttribute('data-brick-num');
                    this.changeBrick(brickNum);
                });
            });
        },

        setupEventListeners: function () {
            // Clear/New Level Button
            document.getElementById('clearButton').addEventListener('click', (e) => {
                e.preventDefault();
                this.initLevel();
                this.drawLevel();
            });

            // Board->Data Button
            document.getElementById('boardToDataButton').addEventListener('click', (e) => {
                e.preventDefault();
                this.drawLevel();
            });

            // Data->Board Button
            document.getElementById('dataToBoardButton').addEventListener('click', (e) => {
                e.preventDefault();
                this.loadData();
            });

            // Brick Type Buttons
            const brickTypeLinks = document.querySelectorAll('.brickTypeSelector');
            brickTypeLinks.forEach(link => {
                link.addEventListener('click', (e) => {
                    e.preventDefault();
                    const type = link.getAttribute('data-type');
                    this.setTypeSelected(type);
                });
            });

            // Powerup Dropdown
            document.getElementById('html_powerupType').addEventListener('change', () => {
                this.setPowerupSelected();
            });
        }
    };

    // Editor starten
    LevelEditor.init();
});