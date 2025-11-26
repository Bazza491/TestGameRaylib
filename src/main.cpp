#include <iostream>
#include "raylib.h"
#include "raymath.h"

#include "Common.h"
#include "world/EnvItem.h"
#include "Player.h"
#include "world/World.h"

//#include "resources/guns/SpellStorage.h"
#include "guns/Wand.h"
#include "gui/GuiManager.h"
#include "gui/items/HealthBar.h"
#include "gui/items/ManaBar.h"
#include "gui/items/StaminaBar.h"
#include "gui/items/WandInventory.h"

//#include <iostream>
//#include <string>


//----------------------------------------------------------------------------------
// Module functions declaration
//----------------------------------------------------------------------------------
void UpdateCameraCenter(Camera2D* camera, const Player &player, float deltaTime,
                        int gameScreenWidth, int gameScreenHeight);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    //region Window
    const int screenWidth = 1280;
    const int screenHeight = 720;
    int gameScreenWidth = 1280;
    int gameScreenHeight = 720;

    bool closed = false;
    float unit = (float) gameScreenHeight / 10.0f;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Noita clone");
    SetWindowMinSize(320, 240);
    MaximizeWindow();
    //endregion

    //region Textures
    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use

    // Load all Textures
    Texture2D numbers = LoadTexture("resources/textures/Numbers.png");
//    Texture2D startWand1 = LoadTexture("");
//    Texture2D startWand2 = LoadTexture("");
    //endregion

    //region Sound
    // Load all Sounds
    InitAudioDevice();

    // Load all Music
    Music music[] = {
            LoadMusicStream("resources/music/UPBEAT-ELECTRO-POP-RELAXING_AdobeStock_535350122_preview.mp3"),
            LoadMusicStream("resources/music/TECHNO-GROOVE-ELECTRO_AdobeStock_539020855_preview (2).mp3"),
            LoadMusicStream("resources/music/CRIME-SPY-GROOVE_AdobeStock_540814030_preview.mp3")
    };
    int currentSong = 0;
    int musicLength = sizeof(music)/sizeof(music[0]);
    // Play starting music
    PlayMusicStream(music[currentSong]);
    SetAudioStreamVolume(music[currentSong].stream, 1.0f);

    // Load all SFX
    // TODO load sfx here
    //endregion

    //region Player
    Player player = Player(numbers);
    //endregion

    //region Environment (World singleton)
    World& world = World::getInstance();
    world.loadWorld("../resources/data/world1.json");
    //endregion

    //region gui elements (gui singleton)
    GuiManager& gui = GuiManager::getInstance();
    gui.add<WandInventory>(&player);
    gui.add<HealthBar>(&player);
    gui.add<ManaBar>(&player);
    gui.add<StaminaBar>(&player);
    //endregion

    //region Camera
    Camera2D camera = { 0 };
    camera.target = player.getPos();
    camera.offset = {(float)gameScreenWidth/2.0f, (float)gameScreenHeight/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    //endregion


    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    //region Game main menu
    for (bool gameStarted = false; !gameStarted && !closed; ) {
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
        float deltaTime = GetFrameTime();

        // Update virtual mouse (clamped mouse value behind game screen)
        Vector2 mouse = GetMousePosition();
        Vector2 virtualMouse = { 0 };
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth*scale))*0.5f)/scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight*scale))*0.5f)/scale;
        virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)gameScreenWidth, (float)gameScreenHeight });
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);
        ClearBackground(RAYWHITE);

        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, GRAY);


        const char *startText = "Start Game";
        int fontSize = 80;
        Vector2 textSize = MeasureTextEx(GetFontDefault(), startText, fontSize, 1);
        Rectangle gameStart = {((float) gameScreenWidth/2)-(textSize.x/2), ((float) gameScreenHeight/2)-(textSize.y/2), textSize.x, textSize.y};


        DrawRectangleRec(gameStart, GREEN);
        DrawTextEx(GetFontDefault(), startText, {gameStart.x, gameStart.y},
                   fontSize, 1, {70, 70, 70, 255});

        DrawText(TextFormat("Text Width: %i", textSize.x), 10, 30, 20,GRAY);


        EndTextureMode();

//        UpdateMusicStream(music[currentSong]);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                       (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                                    (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(virtualMouse, gameStart)) gameStarted = true;
        if (WindowShouldClose()) {
            closed = true;
            break;
        }
    }
    //endregion

    //--------------------------------------------------------------------------------------
    // Main game loop
    //--------------------------------------------------------------------------------------
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        if (closed) break;
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------

        // Initialise frame data
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);
        float deltaTime = GetFrameTime();

        // Play sounds and animations

        // Update Music
        // UpdateMusicStream(music[currentSong]);

        //Update Camera/Window
        UpdateCameraCenter(&camera, player, deltaTime, gameScreenWidth, gameScreenHeight);
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        Vector2 mouse = GetMousePosition();

        // Convert real mouse → virtual game mouse
        Vector2 virtualMouse;
        virtualMouse.x = (mouse.x - (GetScreenWidth() - (gameScreenWidth * scale)) * 0.5f) / scale;
        virtualMouse.y = (mouse.y - (GetScreenHeight() - (gameScreenHeight * scale)) * 0.5f) / scale;
        // Now convert virtual → world space
        Vector2 mouseWorldPos = GetScreenToWorld2D(virtualMouse, camera);

        // Update player.
        player.update(deltaTime, mouseWorldPos);

        gui.update(deltaTime, virtualMouse); // update gui elements
        world.update(); // update world items


        // Cast spell on left mouse button press
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            player.cast(); //TODO Cast cooldown managed by Wand.h
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);

        BeginMode2D(camera);
        ClearBackground((Color){ 255, 233, 173, 255 });
        player.draw(1.0f, 1.0f, deltaTime);

        world.draw();   // draw all items

        EndMode2D();


        // Draw debug panel TODO: Temporary
        float debugPanelHeight = 180;
        float debugPanelWidth = 300;
        const float GUI_MARGIN = 20;

        Rectangle debugPanel = {
                GUI_MARGIN,
                gameScreenHeight - GUI_MARGIN - debugPanelHeight,
                debugPanelWidth,
                debugPanelHeight
        };

        DrawText(TextFormat("Player Velocity: [%i , %i]",
                            (int)player.getVelX(), (int)player.getVelY()), debugPanel.x + GUI_MARGIN, debugPanel.y + GUI_MARGIN, GUI_MARGIN, GRAY);
        DrawText(TextFormat("Player Position: [%i , %i]",
                            (int)player.getPos().x, (int)player.getPos().y), debugPanel.x + GUI_MARGIN, debugPanel.y + 3*GUI_MARGIN, GUI_MARGIN, GRAY);
        DrawText(TextFormat("FPS: %i", GetFPS()), debugPanel.x + GUI_MARGIN, debugPanel.y + 5*GUI_MARGIN, GUI_MARGIN, GRAY);
        DrawText(TextFormat("World items size: %i",
                            world.getItems().size()), debugPanel.x + GUI_MARGIN, debugPanel.y + 7*GUI_MARGIN, GUI_MARGIN, GRAY);

        gui.draw(); // draw gui elements

        EndTextureMode();



        //----------------------------------------------------------------------------------
        // Draw everything in the render texture to the screen, properly scaled
        //----------------------------------------------------------------------------------

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                       (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                                    (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }
    //--------------------------------------------------------------------------------------
    // De-Initialization
    //--------------------------------------------------------------------------------------
    // save game state
    world.saveWorld("../resources/data/world1.json");
    // Unload textures
    UnloadTexture(numbers);
//    UnloadTexture(startWand1);
//    UnloadTexture(startWand2);
    // Unload sounds

    // Unload music
    for (int i = 0; i < musicLength; i++) {
        UnloadMusicStream(music[i]);
    }

    // Destroy objects

    // Close devices and window
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
extern float camVelX = 0;
extern float camVelY = 0;
void UpdateCameraCenter(Camera2D* camera, const Player &player, float deltaTime,
                        int gameScreenWidth, int gameScreenHeight) {
    camera->offset = { (float)gameScreenWidth/2.0f, (float)gameScreenHeight/2.0f};
    float targetX = player.getPos().x + player.getHitBox().width/2;
    float targetY = player.getPos().y + player.getHitBox().height/2;

    camera->target.x = SmoothDamp(camera->target.x, targetX, camVelX, 0.2f, 32767.0f, deltaTime);
    camera->target.y = SmoothDamp(camera->target.y, targetY, camVelY, 0.2f, 32767.0f, deltaTime);

}