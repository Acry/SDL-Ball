#!/usr/bin/env python3
import os
import re
import json
from collections import defaultdict


def find_classes(directory='.'):
    classes = defaultdict(list)
    class_pattern = re.compile(r'class\s+(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?')

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h')):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        matches = class_pattern.finditer(content)
                        for match in matches:
                            class_name = match.group(1)
                            base_class = match.group(2)
                            classes[file_path].append({
                                'name': class_name,
                                'base': base_class
                            })
                except UnicodeDecodeError:
                    print(f"Fehler beim Lesen von {file_path}")
    return classes


def generate_html(classes):
    # Konvertiere classes in JSON-Format für JavaScript
    json_data = json.dumps(dict(classes))

    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title>C++ Klassenübersicht</title>
        <script src="https://d3js.org/d3.v7.min.js"></script>
        <style>
            body { margin: 0; font-family: Arial, sans-serif; }
            .container { padding: 20px; }
            .node { cursor: pointer; }
            .link { stroke: #999; stroke-opacity: 0.6; }
            .file { font-weight: bold; fill: #333; }
            .class { fill: #666; }
            .inheritance { fill: #009900; }
            svg { border: 1px solid #ccc; }
            .tooltip { position: absolute; background: white; padding: 5px; border: 1px solid #ccc; }
            
            .zoom-info {
                position: fixed;
                bottom: 10px;
                right: 10px;
                background: rgba(0,0,0,0.7);
                color: white;
                padding: 5px 10px;
                border-radius: 3px;
                font-size: 12px;
            }
              
            .download-buttons {
                position: fixed;
                top: 10px;
                right: 10px;
                display: flex;
                gap: 10px;
            }

            .download-btn {
                padding: 8px 16px;
                background: #4CAF50;
                color: white;
                border: none;
                border-radius: 4px;
                cursor: pointer;
            }

            .download-btn:hover {
                background: #45a049;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h2>Gefundene Klassen: ''' + str(sum(len(c) for c in classes.values())) + '''</h2>
            <div id="graph"></div>
        </div>
        <script>
            const data = ''' + json_data + ''';
            
            const width = window.innerWidth - 40;
            const height = window.innerHeight - 100;
            
            const svg = d3.select("#graph")
                .append("svg")
                .attr("width", width)
                .attr("height", height);
            
            const tooltip = d3.select("body")
                .append("div")
                .attr("class", "tooltip")
                .style("opacity", 0);
            
            // Container für zoom-baren Inhalt
            const g = svg.append("g");
            
            // Zoom-Verhalten hinzufügen
            const zoom = d3.zoom()
                .scaleExtent([0.1, 4])
                .on("zoom", (event) => {
                    g.attr("transform", event.transform);
                });
            
            svg.call(zoom);
            
            const simulation = d3.forceSimulation()
                .force("link", d3.forceLink().id(d => d.id).distance(150))
                .force("charge", d3.forceManyBody().strength(-1000))
                .force("center", d3.forceCenter(width / 2, height / 2))
                .force("x", d3.forceX(width / 2).strength(0.1))
                .force("y", d3.forceY(height / 2).strength(0.1))
                .alphaDecay(0.01)
                .alphaMin(0.001)
                .velocityDecay(0.3);
            
            // Nodes und Links vor ihrer Verwendung definieren
            const nodes = [];
            const links = [];
            
            Object.entries(data).forEach(([file, classes]) => {
                const fileNode = {id: file, type: "file"};
                nodes.push(fileNode);
            
                classes.forEach(c => {
                    const classNode = {id: c.name, type: "class", file: file};
                    nodes.push(classNode);
                    links.push({source: file, target: c.name, type: "contains"});
            
                    if (c.base) {
                        links.push({source: c.name, target: c.base, type: "inherits"});
                    }
                });
            });
            
            // Nur eine Definition von link und node
            const link = g.append("g")
                .selectAll("line")
                .data(links)
                .join("line")
                .attr("class", "link")
                .attr("stroke-dasharray", d => d.type === "inherits" ? "5,5" : "")
                .attr("stroke-width", d => d.type === "inherits" ? 2 : 1);
            
            const node = g.append("g")
                .selectAll("g")
                .data(nodes)
                .join("g")
                .attr("class", "node")
                .on("mouseover", (event, d) => {
                    tooltip.transition()
                        .duration(200)
                        .style("opacity", .9);
                    tooltip.html(d.type === "file" ? `Datei: ${d.id}` : `Klasse: ${d.id}<br/>Datei: ${d.file}`)
                        .style("left", (event.pageX + 10) + "px")
                        .style("top", (event.pageY - 28) + "px");
                })
                .on("mouseout", () => {
                    tooltip.transition()
                        .duration(500)
                        .style("opacity", 0);
                })
                .call(d3.drag()
                    .on("start", dragstarted)
                    .on("drag", dragged)
                    .on("end", dragended));
            
            node.append("circle")
                .attr("r", d => d.type === "file" ? 8 : 5)
                .attr("fill", d => d.type === "file" ? "#ff7f0e" : "#1f77b4");
            
            node.append("text")
                .attr("dx", 12)
                .attr("dy", ".35em")
                .attr("class", d => d.type)
                .text(d => d.type === "file" ? d.id.split("/").pop() : d.id);
            
            simulation
                .nodes(nodes)
                .on("tick", ticked);
            
            simulation.force("link")
                .links(links);

        function ticked() {
            link
                .attr("x1", d => d.source.x)
                .attr("y1", d => d.source.y)
                .attr("x2", d => d.target.x)
                .attr("y2", d => d.target.y);

            node
                .attr("transform", d => `translate(${d.x},${d.y})`);
        }

        function dragstarted(event) {
            if (!event.active) simulation.alphaTarget(0.3).restart();
            event.subject.fx = event.subject.x;
            event.subject.fy = event.subject.y;
        }

        function dragged(event) {
            event.subject.fx = event.x;
            event.subject.fy = event.y;
        }

        function dragended(event) {
            if (!event.active) simulation.alphaTarget(0);
            event.subject.fx = null;
            event.subject.fy = null;
        }
        // Zoom-Hinweis hinzufügen
        document.body.insertAdjacentHTML('beforeend',
            '<div class="zoom-info">Mausrad zum Zoomen, Ziehen zum Verschieben</div>');
    
            // Download-Buttons hinzufügen
            document.body.insertAdjacentHTML('beforeend', `
                <div class="download-buttons">
                    <button class="download-btn" onclick="downloadSVG()">Als SVG speichern</button>
                    <button class="download-btn" onclick="downloadPNG()">Als PNG speichern</button>
                </div>
            `);
            
        // SVG Download Funktion
        function downloadSVG() {
            // Kopie des SVG erstellen
            const originalSvg = document.querySelector('svg');
            const svgCopy = originalSvg.cloneNode(true);
            
            // Aktuelle Transformationen auf die Elemente anwenden
            const g = svgCopy.querySelector('g');
            const transform = g.getAttribute('transform');
            if (transform) {
                const lines = g.querySelectorAll('line');
                const nodes = g.querySelectorAll('.node');
                
                // Transformationsmatrix extrahieren
                const match = transform.match(/matrix\((.*)\)/);
                if (match) {
                    const [a, b, c, d, e, f] = match[1].split(',').map(Number);
                    
                    // Transformationen auf die Linien anwenden
                    lines.forEach(line => {
                        const x1 = parseFloat(line.getAttribute('x1')) * a + e;
                        const y1 = parseFloat(line.getAttribute('y1')) * d + f;
                        const x2 = parseFloat(line.getAttribute('x2')) * a + e;
                        const y2 = parseFloat(line.getAttribute('y2')) * d + f;
                        
                        line.setAttribute('x1', x1);
                        line.setAttribute('y1', y1);
                        line.setAttribute('x2', x2);
                        line.setAttribute('y2', y2);
                    });
                    
                    // Transformationen auf die Knoten anwenden
                    nodes.forEach(node => {
                        const transform = node.getAttribute('transform');
                        const match = transform.match(/translate\((.*?),(.*?)\)/);
                        if (match) {
                            const x = parseFloat(match[1]) * a + e;
                            const y = parseFloat(match[2]) * d + f;
                            node.setAttribute('transform', `translate(${x},${y})`);
                        }
                    });
                }
                
                // Transformation zurücksetzen
                g.removeAttribute('transform');
            }
            
            const svgData = svgCopy.outerHTML;
            const blob = new Blob([svgData], {type: 'image/svg+xml'});
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = 'class-diagram.svg';
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            URL.revokeObjectURL(url);
        }
            
        // PNG Download Funktion
        function downloadPNG() {
            // Zuerst SVG mit korrekten Transformationen erstellen
            const originalSvg = document.querySelector('svg');
            const svgCopy = originalSvg.cloneNode(true);
            
            // Gleiche Transformationen wie in downloadSVG anwenden
            const g = svgCopy.querySelector('g');
            const transform = g.getAttribute('transform');
            if (transform) {
                const lines = g.querySelectorAll('line');
                const nodes = g.querySelectorAll('.node');
                
                const match = transform.match(/matrix\((.*)\)/);
                if (match) {
                    const [a, b, c, d, e, f] = match[1].split(',').map(Number);
                    
                    lines.forEach(line => {
                        const x1 = parseFloat(line.getAttribute('x1')) * a + e;
                        const y1 = parseFloat(line.getAttribute('y1')) * d + f;
                        const x2 = parseFloat(line.getAttribute('x2')) * a + e;
                        const y2 = parseFloat(line.getAttribute('y2')) * d + f;
                        
                        line.setAttribute('x1', x1);
                        line.setAttribute('y1', y1);
                        line.setAttribute('x2', x2);
                        line.setAttribute('y2', y2);
                    });
                    
                    nodes.forEach(node => {
                        const transform = node.getAttribute('transform');
                        const match = transform.match(/translate\((.*?),(.*?)\)/);
                        if (match) {
                            const x = parseFloat(match[1]) * a + e;
                            const y = parseFloat(match[2]) * d + f;
                            node.setAttribute('transform', `translate(${x},${y})`);
                        }
                    });
                }
                g.removeAttribute('transform');
            }
        
            const canvas = document.createElement('canvas');
            const ctx = canvas.getContext('2d');
            const svgData = new XMLSerializer().serializeToString(svgCopy);
            const img = new Image();
        
            canvas.width = svgCopy.width.baseVal.value;
            canvas.height = svgCopy.height.baseVal.value;
        
            img.onload = function() {
                ctx.fillStyle = 'white';
                ctx.fillRect(0, 0, canvas.width, canvas.height);
                ctx.drawImage(img, 0, 0);
                const a = document.createElement('a');
                a.href = canvas.toDataURL('image/png');
                a.download = 'class-diagram.png';
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
            };
        
            img.src = 'data:image/svg+xml;base64,' + btoa(unescape(encodeURIComponent(svgData)));
        }
        </script>
    </body>
    </html>
    '''

    with open('class_overview.html', 'w') as f:
        f.write(html)


if __name__ == '__main__':
    print("Suche nach C++ Klassen...")
    classes = find_classes('../src')
    generate_html(classes)
    print("Visualisierung wurde in class_overview.html erstellt")
