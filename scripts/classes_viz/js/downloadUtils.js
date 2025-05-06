function downloadSVG() {
    const svgElement = document.querySelector('svg');
    const gElement = svgElement.querySelector('g');

    // SVG-Kopie erstellen
    const svgCopy = document.createElementNS("http://www.w3.org/2000/svg", "svg");

    // Style-Element hinzufügen
    const style = document.createElementNS("http://www.w3.org/2000/svg", "style");
    style.textContent = `
        .node { cursor: pointer; }
        .link { stroke: #999; stroke-opacity: 0.6; }
        .file { font-weight: bold; fill: #333; }
        .class { fill: #666; }
        circle { fill: #1f77b4; }
        .node circle[r="8"] { fill: #ff7f0e; }
    `;
    svgCopy.appendChild(style);

    // Originalgröße und Transform beibehalten
    svgCopy.setAttribute("width", svgElement.getAttribute("width"));
    svgCopy.setAttribute("height", svgElement.getAttribute("height"));

    // Inhalt kopieren und Transform beibehalten
    const gCopy = gElement.cloneNode(true);
    svgCopy.appendChild(gCopy);

    // SVG speichern
    const svgData = new XMLSerializer().serializeToString(svgCopy);
    const blob = new Blob([svgData], {type: 'image/svg+xml'});
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'class-diagram.svg';
    a.click();
    URL.revokeObjectURL(url);
}

function downloadPNG() {
    const svgElement = document.querySelector('svg');
    const gElement = svgElement.querySelector('g');

    // SVG-Kopie erstellen
    const svgCopy = document.createElementNS("http://www.w3.org/2000/svg", "svg");

    // Style hinzufügen
    const style = document.createElementNS("http://www.w3.org/2000/svg", "style");
    style.textContent = `
        .node { cursor: pointer; }
        .link { stroke: #999; stroke-opacity: 0.6; }
        .file { font-weight: bold; fill: #333; }
        .class { fill: #666; }
        circle { fill: #1f77b4; }
        .node circle[r="8"] { fill: #ff7f0e; }
    `;
    svgCopy.appendChild(style);

    // Originalgröße und Transform beibehalten
    svgCopy.setAttribute("width", svgElement.getAttribute("width"));
    svgCopy.setAttribute("height", svgElement.getAttribute("height"));

    // Inhalt kopieren und Transform beibehalten
    const gCopy = gElement.cloneNode(true);
    svgCopy.appendChild(gCopy);

    // PNG erzeugen
    const svgData = new XMLSerializer().serializeToString(svgCopy);
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');

    // Original-Dimensionen verwenden
    const width = svgElement.width.baseVal.value;
    const height = svgElement.height.baseVal.value;

    // Höhere Auflösung für bessere Qualität
    const scale_factor = 2;
    canvas.width = width * scale_factor;
    canvas.height = height * scale_factor;
    ctx.scale(scale_factor, scale_factor);

    const img = new Image();
    img.onload = function() {
        ctx.fillStyle = 'white';
        ctx.fillRect(0, 0, width, height);
        ctx.drawImage(img, 0, 0, width, height);
        const a = document.createElement('a');
        a.href = canvas.toDataURL('image/png');
        a.download = 'class-diagram.png';
        a.click();
    };

    img.src = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(svgData)));
}