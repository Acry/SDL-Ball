// Lade JSON-Daten
fetch('data/classes.json')
    .then(response => response.json())
    .then(data => {

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
    .on("click", (event, d) => {

    // ClipboardUtils initialisieren
    const { copyToClipboard, showNotification } = createClipboardUtils();

        try {
            // Text zum Kopieren bestimmen
            let textToCopy;
            if (d.type === "file") {
                textToCopy = "src/" + d.id.split("/src/")[1];
            } else {
                textToCopy = "src/" + d.file.split("/src/")[1];
            }

            // Clipboard-Funktionen nutzen
            copyToClipboard(textToCopy);
            showNotification(textToCopy);

        } catch (err) {
            console.error('Fehler beim Kopieren:', err);
        }
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
})
.catch(error => console.error('Fehler beim Laden der JSON-Daten:', error));