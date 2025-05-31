#include"Brick_old.h"
int nbrick[23][26];
int updated_nbrick[23][26];


class Brick : public GameObject {
public:
    int score; //Hvor meget gir den
    bool destroytowin; // Skal den smadres for at man kan vinde?
    char powerup;
    char type;
    GLfloat fade; //hvor meget brik
    GLfloat fadespeed;
    GLfloat zoomspeed;
    GLfloat zoom;
    bool isdyingnormally;
    bool isexploding; //springer den i luften
    int row; //what row is this brick in
    int bricknum; //brick in this row
    int hitsLeft; //Hvor mange gange skal denne brik rammes før den dør?
    bool justBecomeExplosive; //If this brick just become a explosive one.

    [[nodiscard]] bool n(const int dir) const {
        switch (dir) {
            case 0: //Er der en brik til venstre for dig?
                if (bricknum > 0) {
                    if (nbrick[row][bricknum - 1] != -1)
                        return true;
                }
                break;
            case 1: //Er der en brik til højre for dig?
                if (bricknum < 25) //26
                {
                    if (nbrick[row][bricknum + 1] != -1)
                        return true;
                }
                break;
            case 2: //Er der en brik Ovenpå dig
                if (row > 0) {
                    if (nbrick[row - 1][bricknum] != -1)
                        return true;
                }
                break;
            case 3: //Er der en brik nedenunder dig
                if (row < 22) //23
                {
                    if (nbrick[row + 1][bricknum] != -1)
                        return true;
                }
                break;
            default: ;
        }

        return false;
    }