// Funktion direkt definieren und exportieren
function createClipboardUtils() {
    const copyToClipboard = (text) => {
        const tempInput = document.createElement("input");
        tempInput.value = text;
        document.body.appendChild(tempInput);
        tempInput.select();
        document.execCommand("copy");
        document.body.removeChild(tempInput);
    };

    const showNotification = (text) => {
        d3.selectAll(".copy-notification").remove();

        const notification = d3.select("body")
            .append("div")
            .attr("class", "copy-notification")
            .style("position", "fixed")
            .style("bottom", "20px")
            .style("left", "50%")
            .style("transform", "translateX(-50%)")
            .style("background", "rgba(0,0,0,0.8)")
            .style("color", "white")
            .style("padding", "8px 16px")
            .style("border-radius", "4px")
            .style("pointer-events", "none")
            .style("z-index", "9999")
            .style("opacity", "1")
            .style("font-family", "Arial, sans-serif")
            .text("Pfad kopiert: " + text);

        setTimeout(() => {
            notification.style("opacity", "0")
                .style("transition", "opacity 0.5s ease-in-out");
            setTimeout(() => notification.remove(), 500);
        }, 2000);
    };

    return { copyToClipboard, showNotification };
} // Ende der createClipboardUtils Funktion