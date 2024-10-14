
//MD AKASH HOSSAIN
//2021521460115

#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <iostream>
#include <time.h>
#include <list>
#include <windows.h> // Include for Sleep function

using namespace std;
#pragma comment(lib, "winmm.lib")
#pragma warning(disable:4996)

// Size of the screen
#define WIN_WIDTH 400
#define WIN_HEIGHT 600

// Define the maximum of enemy planes, bullets, and health points
#define MAX_ENEMY 50
#define MAX_BULLET 100
#define MAX_ENEMY_BULLET 50
#define MAX_HEALTH 3

// Define bullet and enemy bullet structure
class Node {
public:
    int x;
    int y;
    int health; // Add health attribute for enemy planes
};

// Player plane
Node myPlane = { 200, 590 };
int health = MAX_HEALTH;  // Player health points

// Store the enemy planes and bullets using arrays
Node enemyPlanes[MAX_ENEMY];
Node bullets[MAX_BULLET];
Node enemyBullets[MAX_ENEMY_BULLET];
bool enemyActive[MAX_ENEMY] = { false };       // Active state of enemy planes
bool bulletActive[MAX_BULLET] = { false };     // Active state of bullets
bool enemyBulletActive[MAX_ENEMY_BULLET] = { false };  // Active state of enemy bullets

// Boss variables
bool bossActive = false;
int bossX, bossY, bossHealth = 5;  // Boss position and health

// Firing control for enemy planes
int enemyFireRate = 0;  // Timer for enemy shooting
int bulletSpeed = 2;    // Decreased speed of enemy bullets

// Function prototypes
void DrawAircraft(int x, int y);
void DrawEnemyAircraft(int x, int y, int health); // Modified to include health
void IsPressKey();
void AddBullet();
void Shoot();
void DetectCollision();
void EnemyShoot();
void CheckEnemyBullet();
void AddEnemy();
void AddBoss();
void DrawHealth();
void MoveEnemiesAndShoot();
void CheckPlayerBulletCollisionWithEnemyBullets(); // Modified function prototype
void StartGame(); // New function prototype
void GameOver(); // New function prototype

// Function to draw the player aircraft
void DrawAircraft(int x, int y) {
    setcolor(BLUE); // Set color for player's aircraft
    rectangle(x + 10, y, x + 22, y + 18);  // Body of the plane
    line(x, y + 8, x + 10, y + 8);         // Left wing
    line(x + 22, y + 8, x + 32, y + 8);    // Right wing
    line(x, y + 8, x + 8, y + 18);         // Left wing tail connection
    line(x + 32, y + 8, x + 24, y + 18);   // Right wing tail connection
    line(x + 10, y, x + 16, y - 8);        // Left side of nose
    line(x + 22, y, x + 16, y - 8);        // Right side of nose
}

// Function to draw enemy aircraft with health bar
void DrawEnemyAircraft(int x, int y, int health) {
    setcolor(RED); // Set color for enemy aircraft
    rectangle(x + 10, y, x + 22, y + 18);  // Body of the enemy plane
    line(x, y + 8, x + 10, y + 8);         // Left wing
    line(x + 22, y + 8, x + 32, y + 8);    // Right wing
    line(x, y + 8, x + 8, y + 18);         // Left wing tail connection
    line(x + 32, y + 8, x + 24, y + 18);   // Right wing tail connection
    line(x + 10, y, x + 16, y - 8);        // Left side of nose
    line(x + 22, y, x + 16, y - 8);        // Right side of nose

    // Draw health bar above enemy aircraft
    int barWidth = 30; // Width of the health bar
    int barHeight = 5; // Height of the health bar
    int healthWidth = (health * barWidth) / MAX_HEALTH; // Calculate health width based on current health

    // Background of the health bar (white)
    setfillcolor(WHITE);
    fillrectangle(x + 10, y - 10, x + 10 + barWidth, y - 10 + barHeight);

    // Current health (green)
    setfillcolor(GREEN);
    fillrectangle(x + 10, y - 10, x + 10 + healthWidth, y - 10 + barHeight);

    // Display health points as text above the health bar
    char healthText[10];
    sprintf(healthText, "%d", health);
    settextcolor(BLACK); // Set text color to black
    //outtextxy(x + 10 + (barWidth / 2) - 5, y - 15, healthText); // Center text above the health bar
}

// Function to start the game
void StartGame() {
    settextcolor(RED);
    outtextxy(200, 300, _T("Game Start!"));
    outtextxy(180, 380, _T("Press ESC "));

    // Wait for the ESC key to start the game
    while (true) {
        if (_kbhit()) {
            char key = getch();
            if (key == 27) { // ESC key
                break; // Exit the StartGame function and start the game loop
            }
        }
    }
    cleardevice(); // Clear the screen before starting the game
}

// Game Over function
void GameOver() {
    settextcolor(RED);
    outtextxy(150, 300, _T("Game Over!"));

    // Wait for the ESC key to exit
    while (true) {
        if (_kbhit()) {
            char key = getch();
            if (key == 27) { // ESC key
                exit(0); // Exit the program
            }
        }
    }
}

// Detection of keyboard signal
void IsPressKey() {
    if (_kbhit()) {  // If any key is pressed
        char key;
        key = getch();  // Keyboard info

        switch (key) {
        case 72:  // Up
            myPlane.y -= 8;  // Vertical movement (can adjust this value as needed)
            break;
        case 80:  // Down
            myPlane.y += 8;  // Vertical movement (can adjust this value as needed)
            break;
        case 75:  // Left
            myPlane.x -= 12;  // Increased horizontal movement to the left
            break;
        case 77:  // Right
            myPlane.x += 12;  // Increased horizontal movement to the right
            break;
        case ' ':  // Space to shoot
            AddBullet();
            break;
        case 27:  // ESC to exit
            exit(0); // Exit the program
        default:
            break;
        }
    }
}

// Function to add a bullet
void AddBullet() {
    for (int i = 0; i < MAX_BULLET; ++i) {
        if (!bulletActive[i]) {
            bullets[i] = { myPlane.x + 16, myPlane.y - 20 };
            bulletActive[i] = true;
            break;
        }
    }
}

// Collision detection between bullets and enemy planes
void Shoot() {
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (!enemyActive[i]) continue;  // Skip the inactive enemy planes
        for (int j = 0; j < MAX_BULLET; ++j) {
            if (!bulletActive[j]) continue;  // Skip the inactive bullets
            // Collision detection
            if (bullets[j].x >= (enemyPlanes[i].x - 10) &&
                bullets[j].x <= (enemyPlanes[i].x + 50) &&
                bullets[j].y >= (enemyPlanes[i].y - 15) &&
                bullets[j].y <= (enemyPlanes[i].y + 30)) {
                // Both the enemy planes and bullets disappear after collision
                enemyPlanes[i].health--; // Decrease enemy health
                bulletActive[j] = false;  // Deactivate bullet

                // If enemy health is zero, deactivate the enemy
                if (enemyPlanes[i].health <= 0) {
                    enemyActive[i] = false; // Remove the enemy plane
                }
                break;
            }
        }
    }
}

// Collision detection between player and enemy bullets or planes
void DetectCollision() {
    // Check for enemy plane collision with player
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (enemyActive[i] &&
            myPlane.x < enemyPlanes[i].x + 50 && myPlane.x + 32 > enemyPlanes[i].x &&
            myPlane.y < enemyPlanes[i].y + 30 && myPlane.y + 18 > enemyPlanes[i].y) {
            // Player hit by enemy plane
            health--;
            enemyActive[i] = false;  // Remove the enemy plane
        }
    }
    // Check for enemy bullets collision with player
    for (int j = 0; j < MAX_ENEMY_BULLET; ++j) {
        if (enemyBulletActive[j] &&
            myPlane.x < enemyBullets[j].x + 5 && myPlane.x + 32 > enemyBullets[j].x &&
            myPlane.y < enemyBullets[j].y + 5 && myPlane.y + 18 > enemyBullets[j].y) {
            // Player hit by enemy bullet
            health--;
            enemyBulletActive[j] = false;  // Remove the bullet
            GameOver(); // Call GameOver function
        }
    }
}

// Enemy shoots bullets directly downwards
void EnemyShoot() {
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (!enemyActive[i]) continue;  // Skip inactive enemies
        for (int j = 0; j < MAX_ENEMY_BULLET; ++j) {
            if (!enemyBulletActive[j]) {
                // Set the bullet's position to the bottom of the enemy plane
                enemyBullets[j] = { enemyPlanes[i].x + 12, enemyPlanes[i].y + 18 }; // Adjusted for front side shooting
                enemyBulletActive[j] = true;
                break;
            }
        }
    }
}

// Move enemies downwards and handle their shooting
void MoveEnemiesAndShoot() {
    enemyFireRate++;  // Increment the fire rate timer
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (enemyActive[i]) {
            // Move enemy downwards
            enemyPlanes[i].y += 1;  // Move down

            // Enemy shoots bullets at a slower rate
            if (enemyFireRate % 300 == 0) {  // Increased interval for shooting
                EnemyShoot();
            }
        }
    }
}

// Check if enemy planes or bullets move out of the screen
void CheckEnemyBullet() {
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (enemyActive[i] && enemyPlanes[i].y >= WIN_HEIGHT)
            enemyActive[i] = false;
    }

    for (int j = 0; j < MAX_BULLET; ++j) {
        if (bulletActive[j] && bullets[j].y <= -10)
            bulletActive[j] = false;
    }

    for (int j = 0; j < MAX_ENEMY_BULLET; ++j) {
        if (enemyBulletActive[j]) {
            // Move enemy bullet straight down
            enemyBullets[j].y += bulletSpeed;  // Decrease speed of enemy bullets
            if (enemyBullets[j].y >= WIN_HEIGHT) {
                enemyBulletActive[j] = false;  // Deactivate bullet if it goes off-screen
            }
        }
    }
}

// Create new enemy planes
void AddEnemy() {
    for (int i = 0; i < MAX_ENEMY; ++i) {
        if (!enemyActive[i]) {  // Find inactive enemy planes
            enemyPlanes[i] = { rand() % (WIN_WIDTH - 50), 0, MAX_HEALTH }; // Initialize health
            enemyActive[i] = true;
            break;
        }
    }
}

// Boss creation
void AddBoss() {
    bossX = (WIN_WIDTH - 100) / 2;
    bossY = 50;  // Start position
    bossActive = true;
}

// Draw the player's health on the screen
void DrawHealth() {
    char healthText[10];
    sprintf(healthText, "Health: %d", health);
    // outtextxy(10, 10, healthText); // Display health
}

// Check collision between player bullets and enemy bullets
void CheckPlayerBulletCollisionWithEnemyBullets() {
    for (int j = 0; j < MAX_BULLET; ++j) {
        if (bulletActive[j]) {  // Check if player's bullet is active
            for (int k = 0; k < MAX_ENEMY_BULLET; ++k) {
                if (enemyBulletActive[k]) {  // Check if enemy bullet is active
                    // Check for collision
                    if (bullets[j].x >= enemyBullets[k].x && bullets[j].x <= enemyBullets[k].x + 5 &&
                        bullets[j].y >= enemyBullets[k].y && bullets[j].y <= enemyBullets[k].y + 5) {
                        // Deactivate enemy bullet
                        enemyBulletActive[k] = false;
                        // Optionally deactivate player's bullet too
                        // bulletActive[j] = false; // Uncomment this line if you want the player's bullet to vanish on collision
                        break;  // Exit inner loop if collision is detected
                    }
                }
            }
        }
    }
}

// Main function
int main() {
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    setcolor(WHITE);  // Set color for drawing

    StartGame(); // Call to wait for ESC to start the game

    // Game loop
    while (true) {
        if (health <= 0) {
            break;  // Game over if health is zero
        }

        // Clear screen and redraw everything
        cleardevice();
        DrawAircraft(myPlane.x, myPlane.y);

        // Handle player inputs
        IsPressKey();
        Shoot();
        DetectCollision();
        CheckEnemyBullet();
        MoveEnemiesAndShoot();  // Move enemies and handle shooting

        // Check player bullets against enemy bullets
        CheckPlayerBulletCollisionWithEnemyBullets();  // Check for bullet collision

        // Add new enemy planes at intervals
        if (rand() % 100 < 5) {
            AddEnemy();
        }

        // Draw enemy planes
        for (int i = 0; i < MAX_ENEMY; ++i) {
            if (enemyActive[i]) {
                DrawEnemyAircraft(enemyPlanes[i].x, enemyPlanes[i].y, enemyPlanes[i].health); // Pass health to the draw function
            }
        }

        // Draw bullets
        setcolor(GREEN); // Set color for player's bullets
        for (int j = 0; j < MAX_BULLET; ++j) {
            if (bulletActive[j]) {
                bullets[j].y -= 10;  // Move bullet up
                circle(bullets[j].x + 5, bullets[j].y + 5, 5); // Draw bullet
            }
        }

        // Draw enemy bullets
        setcolor(RED); // Set color for enemy bullets
        for (int j = 0; j < MAX_ENEMY_BULLET; ++j) {
            if (enemyBulletActive[j]) {
                rectangle(enemyBullets[j].x, enemyBullets[j].y, enemyBullets[j].x + 5, enemyBullets[j].y + 5); // Draw enemy bullet
            }
        }

        // Draw health
        DrawHealth();

        // Control the frame rate
        Sleep(30);  // Adjust this value to control speed
    }

    GameOver(); // Call GameOver function when the game ends
    closegraph();
    return 0;
}