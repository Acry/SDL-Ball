#include <list>
#include "Texture.h"
#include "Text.h"

struct effect_vars {
    int type; //Hvilken slags effekt er det?
    bool coldet; //er der collision detection på denne?
    bool active; //Er denne effekt aktiv
    GLfloat col[3]; //Hvilken farve har effekten?
    GLfloat speed; //Hvor hurtigt bevæger den sig
    GLfloat spread; //Hvor stor er spredningen (i grader)
    GLfloat size; //Skalering af elementerne
    pos rect; //rectangular size (for particleField)
    GLfloat gravity; //Hvor stor er tyndgekraften
    int num; //Hvor mange elementer er der i den
    int life; //hvor mange ms lever den?
    Texture tex; //Texture
    int effectId; //unique id for this effect
};

class EffectsTransist {
    GLfloat opacity;
public:
    Text& text;  // Als Referenz
    int age;
    effect_vars vars;

    // Konstruktor initialisiert die Referenz
    EffectsTransist() : text(Text::getInstance()){}

    void init() {
        age = 0;
        opacity = 0.0f;
        var.transition_half_done = false;
    }

    void draw() {
        age += globalTicksSinceLastDraw;

        if (age < vars.life / 2.0) //første halvdel
        {
            //Gør solid
            opacity += vars.life / 500.0 * globalMilliTicksSinceLastDraw;
        } else {
            var.transition_half_done = 1;
            //Gør trans
            opacity -= vars.life / 500.0 * globalMilliTicksSinceLastDraw;
        }

        glLoadIdentity();
        glDisable(GL_TEXTURE_2D);
        glColor4f(vars.col[0], vars.col[1], vars.col[2], opacity);
        glBegin(GL_QUADS);
        glVertex3f(-1.0f, 1.0f, 0.0);
        glVertex3f(1.0f, 1.0f, 0.0);
        glVertex3f(1.0f, -1.0f, 0.0);
        glVertex3f(-1.0f, -1.0f, 0.0);
        glEnd();
    }
};

class sparkle {
public:
    bool active;
    GLfloat size;
    GLfloat ang;
    int life;
    int lifeleft;
    pos p, v; //position og vel
    effect_vars vars;
    GLfloat bounce, f; //bounce og friktion

    sparkle() {
        bounce = 0;
        f = 0;
        active = true;
    }

    void draw() {
        if (lifeleft > 0) {
            //sanitize
            lifeleft -= globalTicksSinceLastDraw;

            //er vi indenfor skærmen?
            if (p.x > 1.67 || p.y < -1.7 || p.x < -1.67) {
                active = false;
            }

            v.y -= vars.gravity * globalMilliTicksSinceLastDraw;
            v.y -= bounce * globalMilliTicksSinceLastDraw;

            if (v.x < 0) {
                v.x += f * globalMilliTicksSinceLastDraw;
            } else {
                v.x -= f * globalMilliTicksSinceLastDraw;
            }

            p.x += v.x * globalMilliTicksSinceLastDraw;
            p.y += v.y * globalMilliTicksSinceLastDraw;

            glColor4f(vars.col[0], vars.col[1], vars.col[2], (1.0 / static_cast<float>(life)) * static_cast<float>(lifeleft));

            const GLfloat curSize = size / static_cast<float>(life) * static_cast<float>(lifeleft);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, vars.tex.textureProperties.texture);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex3f(p.x - curSize, p.y + curSize, 0.0);
            glTexCoord2f(0, 1);
            glVertex3f(p.x + curSize, p.y + curSize, 0.0);
            glTexCoord2f(1, 1);
            glVertex3f(p.x + curSize, p.y - curSize, 0.0);
            glTexCoord2f(1, 0);
            glVertex3f(p.x - curSize, p.y - curSize, 0.0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        } else {
            active = false;
        }
    }

    void coldet(brick &b) {
        if (p.x > b.posx - b.width && p.x < b.posx + b.width) {
            if (p.y > b.posy - b.height && p.y < b.posy + b.height) {
                //Hvor ramte vi fra?
                if (p.y < b.posy && !b.n(3)) {
                    //bunden
                    v.y *= -1;
                    //p.y = b.posy-b.height-0.01;
                } else if (p.y > b.posy && !b.n(2)) {
                    //toppen
                    p.y = b.posy + b.height;
                    if (v.y < 0) {
                        v.y *= -1;
                        v.y /= 2.0;
                        f += 0.01;
                    }
                } else if (p.x > b.posx && !b.n(1)) {
                    //p.x = b.posx+b.width;
                    //højre
                    if (v.x < 0)
                        v.x *= -1;
                } else if (p.x < b.posx && !b.n(0)) {
                    //p.x = b.posx-b.width;
                    //venstre
                    if (v.x > 0)
                        v.x *= -1;
                }
            }
        }
    }

    void pcoldet(paddle_class &b) {
        if (p.x > b.posx - b.width && p.x < b.posx + b.width) {
            if (p.y > b.posy - b.height && p.y < b.posy + b.height) {
                //Hvor ramte vi fra?
                if (p.y < b.posy) {
                    //bunden
                    p.y = b.posy - b.height;
                }

                if (p.y > b.posy) {
                    //toppen
                    p.y = b.posy + b.height;
                    if (v.y < 0) {
                        v.y *= -1;
                        v.y /= 2.0;
                        f += 0.01;
                    }
                } else if (p.x > b.posx) {
                    p.x = b.posx + b.width;
                    //højre
                    if (v.x < 0)
                        v.x *= -1;
                } else if (p.x < b.posx) {
                    p.x = b.posx - b.width;
                    //venstre
                    if (v.x > 0)
                        v.x *= -1;
                }
            }
        }
    }
};

class particleFieldClass {
    int spawnTimeout; //This is used to spawn sparkles at a random time
    sparkle *sparks;

    void spawnSpark(int sparkNum);

public:
    pos p;
    effect_vars vars;

    void init(effect_vars varsP, pos p);

    void draw();

    void move(pos p);

    void coldet(brick &b);

    void pcoldet(paddle_class &b);

    ~particleFieldClass();
};

void particleFieldClass::init(effect_vars varsP, pos spawnPos) {
    vars = varsP;
    spawnTimeout = 0;
    vars.active = true;
    sparks = new sparkle[vars.num];
    p = spawnPos;

    for (int i = 0; i < vars.num; i++) {
        sparks[i].active = true;
        sparks[i].vars = vars; //NOTE:remove effect_vars from sparkle class?
        spawnSpark(i);
    }
}

particleFieldClass::~particleFieldClass() {
    delete[] sparks; //free the sparks
}

void particleFieldClass::spawnSpark(int sparkNum) {
    const GLfloat angle = (RAD / vars.num - 1) * (random_float(vars.num, 0.0));
    //FIXME: Make this elegant: Choose a random angle (in radients)
    spawnTimeout = rand() % 10;
    sparks[sparkNum].life = rand() % vars.life;
    sparks[sparkNum].lifeleft = sparks[sparkNum].life;
    sparks[sparkNum].v.x = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * sin(angle);
    sparks[sparkNum].v.y = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * cos(angle);
    sparks[sparkNum].size = random_float(vars.size, 0);

    sparks[sparkNum].p = p;

    sparks[sparkNum].p.x += random_float(vars.rect.x, (vars.rect.x / 2.0));
    sparks[sparkNum].p.y += random_float(vars.rect.y, (vars.rect.y / 2.0));
    sparks[sparkNum].active = true;
}

void particleFieldClass::draw() {
    spawnTimeout -= globalTicksSinceLastDraw;

    int t = 0;

    for (int i = 0; i < vars.num; i++) {
        if (sparks[i].active) {
            t++;
            sparks[i].draw();
        }
    }

    vars.life -= globalTicksSinceLastDraw;
    if (t == 0) {
        vars.active = 0;
    }
}

void particleFieldClass::coldet(brick &b) {
    for (int i = 0; i < vars.num; i++) {
        if (sparks[i].active)
            sparks[i].coldet(b);
    }
}

void particleFieldClass::pcoldet(paddle_class &b) {
    for (int i = 0; i < vars.num; i++) {
        if (sparks[i].active)
            sparks[i].pcoldet(b);
    }
}

class effect_class {
    pos spawn_pos;
    sparkle *sparks;

public:
    EffectsTransist transit;
    particleFieldClass *pf;
    effect_vars vars;

    effect_class() {
        vars.active = false; //når den bliver oprettet i hukkommelsen er den ikke i brug endnu
    }

    void init(pos p) {
        int i;
        GLfloat angle = 0.0;
        vars.active = true;
        spawn_pos = p;
        //SDL_Log("Spawned effect type%s at%s,%s with%sms life", vars.type, p.x, p.y, vars.life);

        switch (vars.type) {
            case FX_SPARKS:
                sparks = new sparkle[vars.num];
                if (sparks == nullptr) {
                    SDL_Log("Could not allocate %d sparks", vars.num);
                    i = vars.num + 10;
                }

                for (i = 0; i < vars.num; i++) {
                    sparks[i].size = random_float(vars.size, 0);
                    int life = rand() % vars.life;
                    angle = (RAD / vars.num - 1) * (random_float(vars.num, 0.0));
                    //FIXME: Make this elegant: Choose a random angle (in radients)
                    sparks[i].life = life;
                    sparks[i].lifeleft = life;
                    sparks[i].v.x = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * sin(angle);
                    sparks[i].v.y = (vars.speed * random_float(vars.speed * 2.0, 0.0)) * cos(angle);

                    sparks[i].vars = vars;

                    sparks[i].p = spawn_pos;
                }
                break;

            case FX_TRANSIT:
                transit.vars = vars;
                transit.init();
                break;

            case FX_PARTICLEFIELD:
                pf = nullptr;
                pf = new particleFieldClass;
                pf->init(vars, p);
                break;
            default: ;
        }
    }

    void draw() {
        //find ud af hvor længe der er gået siden sidst
        int i;
        bool stay = false;
        switch (vars.type) {
            case FX_SPARKS:
                for (i = 0; i < vars.num; i++) {
                    if (sparks[i].active) {
                        sparks[i].draw();
                        stay = true;
                    }
                }
                break;

            case FX_TRANSIT:
                transit.draw();
                if (transit.age <= vars.life)
                    stay = true;
                break;

            case FX_PARTICLEFIELD:
                pf->draw();
                if (pf->vars.active)
                    stay = true;
                break;
            default: ;
        }

        //Ugley hack, to prevent transit from flickering when spark effects are deleted.
        if (var.effectnum != -1 && vars.type != FX_TRANSIT)
            stay = true;

        if (!stay) {
            vars.active = false;
            switch (vars.type) {
                case FX_SPARKS:
                    delete[] sparks; //Free sparks again ;)
                    break;

                case FX_PARTICLEFIELD:
                    delete pf; //remove the particlefield
                    break;
                default: ;
            }
        }
    }

    void coldet(brick &b) {
        if (vars.type == FX_SPARKS) {
            for (int i = 0; i < vars.num; i++) {
                if (sparks[i].active) {
                    sparks[i].coldet(b);
                }
            }
        } else if (vars.type == FX_PARTICLEFIELD) {
            pf->coldet(b);
        }
    }

    void pcoldet(paddle_class &b) {
        if (vars.type == FX_SPARKS) {
            for (int i = 0; i < vars.num; i++) {
                if (sparks[i].active) {
                    sparks[i].pcoldet(b);
                }
            }
        } else if (vars.type == FX_PARTICLEFIELD) {
            pf->pcoldet(b);
        }
    }
};

class effectManager {
    effect_vars vars; //denne kopieres over i den næste effekt der bliver spawned
    int effectId; //ever rising number of a spawned effect.

public:
    list<effect_class> effects;

    effectManager() {
        effects.clear();
        effectId = 0;
    }

    void set(int var, GLfloat val) {
        switch (var) {
            case FX_VAR_SPEED:
                vars.speed = val;
                break;
            case FX_VAR_SPREAD:
                vars.spread = val;
                break;
            case FX_VAR_SIZE:
                vars.size = val;
                break;
            case FX_VAR_GRAVITY:
                vars.gravity = val;
            default: ;
        }
    }

    void set(const int var, const int val) {
        switch (var) {
            case FX_VAR_NUM:
                vars.num = val;
                break;
            case FX_VAR_LIFE:
                vars.life = val;
                break;
            case FX_VAR_TYPE:
                vars.type = val;
                break;
            case FX_VAR_COLDET:
                vars.coldet = val;
                break;
            default: ;
        }
    }

    void set(int var, GLfloat r, GLfloat g, GLfloat b) {
        switch (var) {
            case FX_VAR_COLOR:
                vars.col[0] = r;
                vars.col[1] = g;
                vars.col[2] = b;
                break;
            default: ;
        }
    }

    void set(int var, Texture tex) {
        switch (var) {
            case FX_VAR_TEXTURE:
                vars.tex = tex;
                break;
            default: ;
        }
    }

    void set(int var, struct pos p) {
        switch (var) {
            case FX_VAR_RECTANGLE:
                vars.rect = p;
                break;
            default: ;
        }
    }

    //retunerer effektid så der kan checkes om det er aktivt
    int spawn(pos p) {
        effectId++;
        effect_class tempEffect;
        vars.effectId = effectId;
        tempEffect.vars = vars; //kopier det over som er sat op
        tempEffect.init(p);
        effects.push_back(tempEffect);

        return (effectId);
    }

    void draw() {
        for (list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it) {
            it->draw();

            if (!it->vars.active) {
                it = effects.erase(it);
            }
        }
    }

    void coldet(brick &b) {
        if (b.collide && b.active) {
            for (list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it) {
                if (it->vars.coldet)
                    it->coldet(b);
            }
        }
    }

    void pcoldet(paddle_class &b) {
        for (list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it) {
            if (it->vars.coldet)
                it->pcoldet(b);
        }
    }

    int isActive(int id) {
        for (list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it) {
            if (it->vars.effectId == id && it->vars.active) {
                return (1);
            }
        }
        return (0);
    }

    void kill(int id) {
        for (list<effect_class>::iterator it = effects.begin(); it != effects.end(); ++it) {
            if (it->vars.effectId) {
                it->vars.active = 0;
            }
        }
    }
};

class glAnnounceMessageClass {

    int age;
    GLfloat zoom, fade;
    bool fadedir;

public:
    Text& text;  // Referenz statt Instanz
    bool active;
    int life;
    string announcement;
    int font;

    glAnnounceMessageClass() : text(Text::getInstance()) {
        active = true;
        age = 0;
        fade = 0;
        fadedir = false;
        zoom = 0;
    }

    void draw() {
        zoom += 4000.0 / life * globalMilliTicksSinceLastDraw;

        if (fadedir) {
            fade -= 4000.0 / life * globalMilliTicksSinceLastDraw;
        } else {
            fade += 4000.0 / life * globalMilliTicksSinceLastDraw;
        }

        glLoadIdentity();

        glColor4f(1.0, 0.0, 0.0, fade);
        GLfloat s = zoom * 0.85f;
        text.write(announcement, font, true, s, 0.0, 0.0);

        glColor4f(0.0, 1.0, 0.0, fade);
        s = zoom * 0.90f;
        text.write(announcement, font, true, s, 0.0, 0.0);

        glColor4f(0.0, 0.0, 1.0, fade);
        s = zoom * 0.95f;
        text.write(announcement, font, true, s, 0.0, 0.0);

        glColor4f(1.0, 1.0, 1.0, fade);
        s = zoom;
        text.write(announcement, font, true, s, 0.0, 0.0);


        age += globalTicksSinceLastDraw;
        if (age > life * 0.50) {
            fadedir = true;
        }

        if (age > life)
            active = false;
    }
};

#define MAXMSG 10

class glAnnounceTextClass {

    int len; //hvor mange mangler vi at vise
    list<glAnnounceMessageClass> announcement;
    list<glAnnounceMessageClass>::iterator iterator;

public:
    glAnnounceTextClass() {
        len = 0;
    }

    void write(const char *message, const int ttl, const int font) {
        len++;

        announcement.resize(len);

        iterator = announcement.end();
        --iterator;

        iterator->life = ttl;
        iterator->announcement = message;
        iterator->font = font;
    }

    void draw() {
        if (len > 0) {
            iterator = announcement.begin();
            if (iterator->active) {
                iterator->draw();
            } else {
                announcement.erase(iterator);
                len--;
            }
        }
    }
};
