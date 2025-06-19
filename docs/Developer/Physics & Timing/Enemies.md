# Enemies

Ein Spawning-System mit steigendem Schwierigkeitsgrad und verschiedenen Gegnertypen

Vier Bewegungsmuster:

- Static: Wie die Blöcke in Arkanoid
- Patrol: Wie die patrouillierenden Gegner in Bomb Jack
- Chase: Verfolger, die den Spieler jagen
- Flocking: Schwarmverhalten mit Brownsche-Bewegung-Komponente

Progressionssystem mit Wellen und zunehmender Schwierigkeit
Du kannst diese Implementierung entsprechend deiner Spielmechanik anpassen und weitere Muster oder Verhaltensweisen
hinzufügen.

## Gegner-Spawning-System

```c++
class EnemySpawner {
private:
    std::vector<Enemy*> enemies;
    float spawnTimer = 0.0f;
    float spawnInterval = 2.0f;  // Zeit zwischen Spawns
    int waveNumber = 1;
    int maxEnemies = 10;

public:
    void update(float deltaTime) {
        spawnTimer += deltaTime;
        
        // Prüfe, ob Zeit zum Spawnen eines neuen Gegners ist
        if (spawnTimer >= spawnInterval && enemies.size() < maxEnemies) {
            spawnNewEnemy();
            spawnTimer = 0.0f;
            
            // Spawns werden mit fortschreitendem Spiel schneller
            spawnInterval = std::max(0.5f, 2.0f - 0.1f * waveNumber);
        }
        
        // Prüfe, ob eine neue Welle gestartet werden soll
        if (enemies.empty() && spawnTimer > 5.0f) {
            startNewWave();
        }
    }
    
    void spawnNewEnemy() {
        Enemy* enemy = new Enemy();
        
        // Wähle zufälligen Spawn-Typ basierend auf dem Wellenfortschritt
        int spawnType = rand() % (waveNumber + 2);
        
        switch (spawnType) {
            case 0:  // Arkanoid-ähnliche feste Position
                enemy->setPosition(Vector3(rand() % 800, 600, 0));
                enemy->setMovementPattern(MovementPattern::STATIC);
                break;
                
            case 1:  // Bomb Jack-ähnliches Patrouillieren
                enemy->setPosition(Vector3(rand() % 800, rand() % 400 + 200, 0));
                enemy->setMovementPattern(MovementPattern::PATROL);
                break;
                
            case 2:  // Verfolger
                enemy->setPosition(Vector3(rand() % 800, 600, 0));
                enemy->setMovementPattern(MovementPattern::CHASE);
                break;
                
            default:  // Brownsche Bewegung mit Flocking
                enemy->setPosition(Vector3(rand() % 800, rand() % 200 + 400, 0));
                enemy->setMovementPattern(MovementPattern::FLOCKING);
                break;
        }
        
        enemy->setSpeed(50.0f + waveNumber * 10.0f);  // Geschwindigkeit steigt mit Wellen
        enemy->setHealth(waveNumber);
        enemies.push_back(enemy);
    }
    
    void startNewWave() {
        waveNumber++;
        maxEnemies = 10 + waveNumber * 2;
        spawnTimer = 0.0f;
        
        // Bonus-Gegner bei bestimmten Wellen
        if (waveNumber % 5 == 0) {
            spawnBossEnemy();
        }
    }
    
    void removeEnemy(Enemy* enemy) {
        auto it = std::find(enemies.begin(), enemies.end(), enemy);
        if (it != enemies.end()) {
            enemies.erase(it);
            delete enemy;
        }
    }
};
```

## Gegner-Bewegungsmuster

```c++
enum class MovementPattern {
    STATIC,    // Arkanoid-Blöcke
    PATROL,    // Bomb Jack-Patrouillen
    CHASE,     // Spieler-Verfolger
    FLOCKING   // Schwarm-Verhalten
};

class Enemy {
private:
    Vector3 position;
    Vector3 velocity;
    MovementPattern pattern;
    float speed;
    int health;
    
    // Für Patrol-Muster
    Vector3 patrolStart;
    Vector3 patrolEnd;
    bool patrolForward = true;
    
    // Für Flocking
    float separationWeight = 1.5f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 1.0f;

public:
    void update(float deltaTime, const Vector3& playerPosition, const std::vector<Enemy*>& allEnemies) {
        switch (pattern) {
            case MovementPattern::STATIC:
                // Nichts tun für statische Gegner
                break;
                
            case MovementPattern::PATROL:
                updatePatrol(deltaTime);
                break;
                
            case MovementPattern::CHASE:
                updateChase(deltaTime, playerPosition);
                break;
                
            case MovementPattern::FLOCKING:
                updateFlocking(deltaTime, allEnemies, playerPosition);
                break;
        }
        
        // Position aktualisieren
        position += velocity * deltaTime;
        
        // In Spielfeld halten
        keepInBounds();
    }
    
    void updatePatrol(float deltaTime) {
        // Zwischen Start- und Endpunkt patrouillieren
        Vector3 targetPoint = patrolForward ? patrolEnd : patrolStart;
        Vector3 direction = (targetPoint - position).normalized();
        
        velocity = direction * speed;
        
        // Umdrehen, wenn Zielpunkt erreicht
        if ((targetPoint - position).lengthSquared() < 25.0f) {
            patrolForward = !patrolForward;
        }
    }
    
    void updateChase(float deltaTime, const Vector3& playerPosition) {
        // Spieler verfolgen, aber nicht zu schnell
        Vector3 direction = (playerPosition - position).normalized();
        velocity = direction * speed;
    }
    
    void updateFlocking(float deltaTime, const std::vector<Enemy*>& allEnemies, const Vector3& playerPosition) {
        Vector3 separation(0, 0, 0);
        Vector3 alignment(0, 0, 0);
        Vector3 cohesion(0, 0, 0);
        Vector3 attraction(0, 0, 0);
        
        int neighborCount = 0;
        
        // Berechne Flocking-Kräfte
        for (const Enemy* other : allEnemies) {
            if (other == this) continue;
            
            Vector3 toOther = other->position - position;
            float distance = toOther.length();
            
            if (distance < 50.0f) {
                separation -= toOther.normalized() / std::max(distance, 1.0f);
            }
            
            if (distance < 150.0f) {
                alignment += other->velocity;
                cohesion += other->position;
                neighborCount++;
            }
        }
        
        // Schwache Anziehung zum Spieler für etwas Bedrohung
        attraction = (playerPosition - position).normalized() * 0.3f;
        
        // Kräfte anwenden
        if (neighborCount > 0) {
            alignment = (alignment / neighborCount).normalized();
            cohesion = ((cohesion / neighborCount) - position).normalized();
        }
        
        // Zufällige Bewegung hinzufügen (vereinfachte Brownsche Bewegung)
        Vector3 randomForce(
            (float)(rand() % 200 - 100) / 100.0f,
            (float)(rand() % 200 - 100) / 100.0f,
            0.0f
        );
        randomForce.normalize();
        
        // Alle Kräfte kombinieren
        Vector3 totalForce = separation * separationWeight +
                            alignment * alignmentWeight +
                            cohesion * cohesionWeight +
                            attraction +
                            randomForce * 0.2f;
        
        // Kraft auf Bewegung anwenden
        velocity = totalForce.normalized() * speed;
    }
    
    void setMovementPattern(MovementPattern newPattern) {
        pattern = newPattern;
        
        // Spezielle Initialisierung je nach Muster
        if (pattern == MovementPattern::PATROL) {
            patrolStart = position;
            patrolEnd = Vector3(position.x + rand() % 300 - 150, position.y + rand() % 200 - 100, position.z);
        }
    }
    
    // Getter und Setter
    void setPosition(const Vector3& pos) { position = pos; }
    void setSpeed(float s) { speed = s; }
    void setHealth(int h) { health = h; }
    Vector3 getPosition() const { return position; }
    int getHealth() const { return health; }
};
```

## Bewegungsmuster der Gegner in Bomb Jack

In Bomb Jack bewegen sich die Gegner auf einem statischen, nicht scrollenden Spielfeld und haben unterschiedliche
Verhaltensweisen, die das Einsammeln der Bomben durch den Spieler erschweren. Die wichtigsten Bewegungsmuster der Gegner
basierend auf verfügbaren Informationen:

Allgemeine Bewegung:
Gegner wie Vögel, Roboter, Ufos oder Mumien erscheinen an verschiedenen Stellen des Levels und bewegen sich flüssig über
das Spielfeld. Sie verfolgen den Spieler (Jack) teilweise gezielt, insbesondere die fliegende Möwe, die horizontal und
vertikal Jacks Position ansteuert.

Nach dem Erscheinen gibt es eine kurze Verzögerung in der Kollisionsabfrage, sodass Spieler Gegner in der Anfangsphase
ohne Schaden berühren können.

Mit steigendem Level (ab Level 6) erhöhen sich die Anzahl und Geschwindigkeit der Gegner, was die Navigation erschwert.

Spezifische Gegnertypen:
Vögel/Möwen: Starten an unterschiedlichen Positionen und verfolgen Jack aktiv, indem sie seine Bewegungen nachahmen.
Ihre Flugbahnen sind oft unvorhersehbar, da sie sowohl horizontal als auch vertikal agieren.

Mumien und andere Gegner: Sobald sie den Boden des Bildschirms erreichen, können sie sich in andere Formen wie fliegende
Untertassen oder schwebende Orbs verwandeln, die unregelmäßig über das Spielfeld schweben. Diese Transformation macht
ihre Bewegungen schwerer vorhersehbar.

Roboter/Ufos: Bewegen sich in variablen Mustern, oft in Wellen oder Zickzack-Linien, und können Jack bei Kontakt ein
Leben abziehen.

Besondere Mechaniken:
Gegner können durch das Einsammeln eines Powerballs (P) vorübergehend in punktespendende Münzen oder „lächelnde Sonnen“
verwandelt werden, wodurch sie für einige Sekunden unschädlich sind. Nach Ablauf dieser Zeit respawnen sie an ihren
ursprünglichen Startpositionen.

Die Bewegungen der Gegner werden in höheren Levels durch zusätzliche Plattformen und Wände komplexer, da diese die
Navigation einschränken und Gegner in engen Bereichen konzentrieren können.

## Bewegungsmuster der Gegner in Arkanoid

In Arkanoid, einem Breakout-Klon, sind die Gegner (oft als „Aliens“ oder „gegnerische Raumschiffe“ bezeichnet) keine
primäre Bedrohung wie in Bomb Jack, sondern dienen dazu, den Spieler zu stören, indem sie den Ball ablenken oder das
Gameplay erschweren.

Ihre Bewegungsmuster sind wie folgt:
Allgemeine Bewegung:
Gegner erscheinen in den meisten der 32 Levels (außer im finalen Level 33) und bewegen sich in vorgegebenen Bahnen über
das Spielfeld. Sie lenken den Ball ab, wenn er sie trifft, was die Berechnung des Abprallwinkels erschwert.

Sie können durch den Ball oder den Schläger (Vaus) zerstört werden, was Bonuspunkte einbringt, aber ihr Hauptzweck ist
es, den Spieler zu irritieren.

Die Gegner bewegen sich meist horizontal oder in sanften Kurven und tauchen periodisch auf, abhängig vom Fortschritt im
Level.

Spezifische Gegnertypen:
Kleine Raumschiffe/Aliens: Diese bewegen sich in Schwärmen oder einzeln und folgen einfachen, wiederholenden Mustern,
wie z. B. hin- und herfliegen oder in Wellenbewegungen über das Spielfeld ziehen. Ihre Animationen sind flüssig, und sie
sind darauf ausgelegt, den Ball unvorhersehbar umzulenken.

Größere Gegner: Manche Gegner sind robuster und benötigen mehrere Treffer, um zerstört zu werden, ähnlich wie bestimmte
Blöcke. Sie bewegen sich langsamer, aber ihre Positionen können strategisch platziert sein, um den Ball in ungünstige
Bahnen zu lenken.

Endgegner (DOH):
Im finalen Level 33 tritt der Spieler gegen den Boss „DOH“ an, einen „Dimensions-Veränderer“. DOH bleibt stationär, muss
aber etwa 25 Mal getroffen werden, um zerstört zu werden. Es gibt keine zusätzlichen Gegner oder Power-ups in diesem
Level, was den Fokus auf präzises Zielen legt.
https://www.youtube.com/watch?v=xQkiGBag-dE

DOH hat keine Bewegung, sondern ist ein festes Ziel, das durch wiederholte Balltreffer besiegt wird. Seine Größe und
Position machen es schwierig, den Ball gezielt zu steuern.

Besondere Mechaniken:
Gegner in Arkanoid beeinflussen das Gameplay indirekt, indem sie den Ball ablenken, aber sie greifen den Spieler nicht
aktiv an. Ihre Bewegungsmuster sind vorhersehbarer als in Bomb Jack, da sie meist festen Bahnen folgen.

Power-ups, die aus zerstörten Blöcken fallen, können die Interaktion mit Gegnern erleichtern, z. B. durch Laserkanonen,
die Gegner schneller eliminieren, oder durch einen größeren Schläger, der das Abfangen des Balls vereinfacht.
