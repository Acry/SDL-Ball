class Hud {
    SpriteSheetAnimation texBall;
    TextManager &text; // Referenz auf das Singleton
    // For the hud text
    int ticksSinceLastClockCheck;
    time_t nixTime; //Seconds since epoch
    tm timeStruct; //Time struct
    char clockString[13]; //Clock: 00:00\0
    SpriteSheetAnimation *texPowerup; //Pointer to array of powerup textures
    int shopItemSelected;
    shopItemStruct item[NUMITEMSFORSALE];
    bool shopItemBlocked[NUMITEMSFORSALE]; //One can only buy each powerup one time each life/level

public:
    Hud(SpriteSheetAnimation texB, SpriteSheetAnimation texPo[]): texBall(texB),
                                                                  text(TextManager::getInstance()),
                                                                  ticksSinceLastClockCheck(1001),
                                                                  texPowerup(texPo),
                                                                  shopItemSelected(0) {
        item[0].type = PO_LASER;
        item[0].price = 600;
        item[1].type = PO_NORMALBALL;
        item[1].price = 750;
        item[2].type = PO_BIGBALL;
        item[2].price = 800;
        item[3].type = PO_AIMHELP;
        item[3].price = 900;
        item[4].type = PO_GROWPADDLE;
        item[4].price = 960;
        item[5].type = PO_MULTIBALL;
        item[5].price = 980;
        item[6].type = PO_EXPLOSIVE_GROW;
        item[6].price = 990;
        item[7].type = PO_EXPLOSIVE;
        item[7].price = 1000;
        item[8].type = PO_GLUE;
        item[8].price = 1000;
        item[9].type = PO_EASYBRICK;
        item[9].price = 2000;
        item[10].type = PO_GUN;
        item[10].price = 3000;
        item[11].type = PO_THRU;
        item[11].price = 4000;
        item[12].type = PO_LIFE;
        item[12].price = 6000;
    }

    void draw() {
        int i;
        glLoadIdentity();
        //Draw lives left.
        glColor4f(texBall.textureProperties.glTexColorInfo[0], texBall.textureProperties.glTexColorInfo[1],
                  texBall.textureProperties.glTexColorInfo[2],
                  texBall.textureProperties.glTexColorInfo[3]);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texBall.textureProperties.texture);
        texBall.play();
        glBegin(GL_QUADS);
        for (i = 0; i < player.lives - 1; i++) {
            constexpr float size = 0.05f; // Größere Bälle für bessere Sichtbarkeit
            constexpr float space = 0.05f; // Mehr Abstand zwischen den Bällen
            constexpr float y = 1.0f - space; // Etwas tiefer von der oberen Kante

            // Bälle werden von rechts nach links gezeichnet
            glTexCoord2f(texBall.texturePosition[0], texBall.texturePosition[1]);
            glVertex3f(0.9f - space * i, y, 0.0); // Oben rechts
            glTexCoord2f(texBall.texturePosition[2], texBall.texturePosition[3]);
            glVertex3f(0.9f - size - space * i, y, 0.0); // Oben links
            glTexCoord2f(texBall.texturePosition[4], texBall.texturePosition[5]);
            glVertex3f(0.9f - size - space * i, y - size, 0.0); // Unten links
            glTexCoord2f(texBall.texturePosition[6], texBall.texturePosition[7]);
            glVertex3f(0.9f - space * i, y - size, 0.0); // Unten rechts
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        if (setting.showClock) {
            ticksSinceLastClockCheck += globalTicksSinceLastDraw;
            if (ticksSinceLastClockCheck > 1000) {
                ticksSinceLastClockCheck = 0;
                time(&nixTime);
                timeStruct = *(localtime(&nixTime));
                sprintf(clockString, "%02i:%02i", timeStruct.tm_hour, timeStruct.tm_min);
            }
            text.write(clockString, FONT_INTRODESCRIPTION, false, 1.5f, -0.92f,
                       -0.97f);
        }

        //Draw the "shop"
        //First, find out how many items the player can afford, so we can center them
        int canAfford = 0;
        for (i = 0; i < NUMITEMSFORSALE; i++) {
            if (item[i].price <= player.coins) {
                canAfford++;
            }
        }

        if (shopItemSelected > canAfford || shopItemSelected < 0) {
            shopItemSelected = canAfford - 1;
        }

        const GLfloat shopListStartX = -(0.11f * canAfford / 1.0f);
        if (gVar.shopNextItem) {
            gVar.shopNextItem = false;
            shopItemSelected++;

            if (shopItemSelected > canAfford - 1) {
                shopItemSelected = 0;
            }
        } else if (gVar.shopPrevItem) {
            gVar.shopPrevItem = false;
            shopItemSelected--;

            if (shopItemSelected < 0) {
                shopItemSelected = canAfford - 1;
                if (shopItemSelected < 0) {
                    shopItemSelected = 0;
                }
            }
        } else if (gVar.shopBuyItem) {
            gVar.shopBuyItem = false;
            if (item[shopItemSelected].price <= player.coins && !shopItemBlocked[shopItemSelected]) {
                position a, b;
                a.x = shopListStartX + (0.11f * shopItemSelected);
                a.y = 1.15f;
                b.x = 0.0f;
                b.y = 0.0f;
                powerupManager.spawn(a, b, item[shopItemSelected].type);
                player.coins -= item[shopItemSelected].price;
                shopItemBlocked[shopItemSelected] = true;
                gVar.shopNextItem = true;
                soundManager.add(SND_BUY_POWERUP, 0.0f);
            }
        }
        glLoadIdentity();
        glTranslatef(shopListStartX, 0.9f, 0.0f);
        for (i = 0; i < canAfford; i++) {
            if (i == shopItemSelected) {
                if (shopItemBlocked[i]) {
                    glColor4f(GL_FULL_RED);
                } else {
                    glColor4f(1.0, 1.0, 1.0, 1.0);
                }
            } else {
                if (shopItemBlocked[i]) {
                    glColor4f(1.0, 0.0, 0.0, 0.4);
                } else {
                    glColor4f(1.0, 1.0, 1.0, 0.4);
                }
            }
            texPowerup[item[i].type].play();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texPowerup[item[i].type].textureProperties.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[0], texPowerup[item[i].type].texturePosition[1]);
            glVertex3f(-0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[2], texPowerup[item[i].type].texturePosition[3]);
            glVertex3f(0.055, 0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[4], texPowerup[item[i].type].texturePosition[5]);
            glVertex3f(0.055, -0.055, 0.00);
            glTexCoord2f(texPowerup[item[i].type].texturePosition[6], texPowerup[item[i].type].texturePosition[7]);
            glVertex3f(-0.055, -0.055, 0.00);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glTranslatef(0.11, 0.0, 0.0f);
        }
    }

    void clearShop() {
        for (bool &i: shopItemBlocked) {
            i = false;
        }
    }
};