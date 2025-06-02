if (bricks[brick_index].type == BrickType::Base) {
// RGB-Werte aus den nächsten 6 Zeichen extrahieren (RRGGBB Format)
std::array<uint8_t, 3> rgb;
for (int i = 0; i < 3; i++) {
std::string hex_str{line[ch + 2 + i*2], line[ch + 3 + i*2]};
rgb[i] = static_cast<uint8_t>(std::stoul(hex_str, nullptr, 16));
}

    // RGB-Werte normalisieren (0-1)
    constexpr float normalize = 1.0f / 255.0f;
    bricks[brick_index].color = {
        rgb[0] * normalize,
        rgb[1] * normalize, 
        rgb[2] * normalize,
        1.0f
    };
    
    ch += 8; // RRGGBB Format überspringen

} else {
ch += 2; // Standard 2-Zeichen Format
}

___

            // if (brick.type == BrickType::Base) {
            //     // Nächste 6 Zeichen sind Hex-RGB
            //     std::string rgb = line.substr(ch + 2, 6);
            //     float r = std::stoi(rgb.substr(0,2), nullptr, 16) / 255.0f;
            //     float g = std::stoi(rgb.substr(2,2), nullptr, 16) / 255.0f;
            //     float b = std::stoi(rgb.substr(4,2), nullptr, 16) / 255.0f;
            //     brick.color = {r, g, b, 1.0f};
            //     ch += 8; // 2 für LD + 6 für RGB
            // }
            // Hier ggf. weitere Brick-Attribute setzen (z.B. Typ, Powerup)