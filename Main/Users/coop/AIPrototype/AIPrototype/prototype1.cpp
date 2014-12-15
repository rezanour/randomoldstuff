#include "game.h"
#include "prototype1.h"

using namespace DirectX;

const float MaxBlackObjectSpeed       = 0.5f;
const float BlackObjectTurnSpeed      = 0.005f;
const float BlackObjectChaseDistance  = 250.0f;
const float BlackObjectCaughtDistance = 32.0f;
const float BlackObjectHysteresis     = 15.0f;
const float MaxObjectSpeed      = 0.5f;
const float ObjectTurnSpeed     = 0.005f;
const float ObjectEvadeDistance = 125.0f;
const float ObjectHysteresis    = 15.0f;

Prototype1::Prototype1()
{
}

Prototype1::~Prototype1()
{
}

void Prototype1::OnLoadContent()
{
    arrowTexture = GetSprite(L"arrow.png");
    squareTexture = GetSprite(L"square.png");

    for (size_t i = 0; i < 30; i++)
    {
        std::shared_ptr<TestObject> newTestObject = std::make_shared<TestObject>(
            (float)(rand() % GetScreenWidth()),
            (float)(rand() % GetScreenHeight()),
            (float)arrowTexture->GetWidth(),
            (float)arrowTexture->GetHeight());
        objects.push_back(newTestObject);
    }

    blackTestObject = std::make_shared<TestObject>(
            (float)(rand() % GetScreenWidth()),
            (float)(rand() % GetScreenHeight()),
            (float)arrowTexture->GetWidth(),
            (float)arrowTexture->GetHeight());

    greenTestObject = std::make_shared<TestObject>(
            (float)(rand() % GetScreenWidth()),
            (float)(rand() % GetScreenHeight()),
            (float)arrowTexture->GetWidth(),
            (float)arrowTexture->GetHeight());
}

void Prototype1::UpdateBlackTestObject(float elapsedTime)
{
    float chaseThreshold = BlackObjectChaseDistance;
    float caughtThreshold = BlackObjectCaughtDistance;

    if (blackTestObject->state == AiState::Wander)
    {
        chaseThreshold -= BlackObjectHysteresis / 2;
    }
    else if (blackTestObject->state == AiState::Chasing)
    {
        chaseThreshold += BlackObjectHysteresis / 2;
        caughtThreshold -= BlackObjectHysteresis / 2;
    }
    else if (blackTestObject->state == AiState::Caught)
    {
        caughtThreshold += BlackObjectHysteresis / 2;
    }

    // Compare distances to determine the next state
    float distanceFromGreenTestObject = Vector2::Distance(blackTestObject->position, greenTestObject->position);
    if (distanceFromGreenTestObject > chaseThreshold)
    {
        blackTestObject->state = AiState::Wander;
    }
    else if (distanceFromGreenTestObject > caughtThreshold)
    {
        blackTestObject->state = AiState::Chasing;
    }
    else
    {
        blackTestObject->state = AiState::Caught;
    }

    // Process current state
    float currentTankSpeed;
    if (blackTestObject->state == AiState::Chasing)
    {
        blackTestObject->orientation = TurnToFace(blackTestObject->position, /*target to face*/greenTestObject->position, blackTestObject->orientation, BlackObjectTurnSpeed);
        currentTankSpeed = MaxBlackObjectSpeed;
    }
    else if (blackTestObject->state == AiState::Wander)
    {
        Wander(blackTestObject->position, blackTestObject->wanderDirection, blackTestObject->orientation, BlackObjectTurnSpeed);
        currentTankSpeed = .25f * MaxBlackObjectSpeed;
    }
    else
    {
        currentTankSpeed = 0.0f;
    }

    // Construct a heading vector and move forward
    Vector2 heading((float)cos(blackTestObject->orientation), (float)sin(blackTestObject->orientation));
    blackTestObject->position += heading * currentTankSpeed;
}

void Prototype1::Wander(Vector2 position, Vector2& wanderDirection,
            float& orientation, float turnSpeed)
{
    // The wander effect is accomplished by having the character aim in a random
    // direction. Every frame, this random direction is slightly modified.
    // Finally, to keep the characters on the center of the screen, we have them
    // turn to face the screen center. The further they are from the screen
    // center, the more they will aim back towards it.

    // the first step of the wander behavior is to use the random number
    // generator to offset the current wanderDirection by some random amount.
    // .25 is a bit of a magic number, but it controls how erratic the wander
    // behavior is. Larger numbers will make the characters "wobble" more,
    // smaller numbers will make them more stable. we want just enough
    // wobbliness to be interesting without looking odd.
    wanderDirection.x += Lerp(-.25f, .25f, (float)rand());
    wanderDirection.y += Lerp(-.25f, .25f, (float)rand());

    // we'll renormalize the wander direction, ...
    Vector2 zeroV(0.0f);
    if (wanderDirection != zeroV)
    {
        wanderDirection.Normalize();
    }
    // ... and then turn to face in the wander direction. We don't turn at the
    // maximum turning speed, but at 15% of it. Again, this is a bit of a magic
    // number: it works well for this sample, but feel free to tweak it.
    orientation = TurnToFace(position, position + wanderDirection, orientation,
        .15f * turnSpeed);

    // next, we'll turn the characters back towards the center of the screen, to
    // prevent them from getting stuck on the edges of the screen.
    Vector2 screenCenter((float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f);

    // Here we are creating a curve that we can apply to the turnSpeed. This
    // curve will make it so that if we are close to the center of the screen,
    // we won't turn very much. However, the further we are from the screen
    // center, the more we turn. At most, we will turn at 30% of our maximum
    // turn speed. This too is a "magic number" which works well for the sample.
    // Feel free to play around with this one as well: smaller values will make
    // the characters explore further away from the center, but they may get
    // stuck on the walls. Larger numbers will hold the characters to center of
    // the screen. If the number is too large, the characters may end up
    // "orbiting" the center.
    float distanceFromScreenCenter = Vector2::Distance(screenCenter, position);
    float MaxDistanceFromScreenCenter = min(screenCenter.y, screenCenter.x);

    float normalizedDistance =
        distanceFromScreenCenter / MaxDistanceFromScreenCenter;

    float turnToCenterSpeed = .3f * normalizedDistance * normalizedDistance *
        turnSpeed;

    // once we've calculated how much we want to turn towards the center, we can
    // use the TurnToFace function to actually do the work.
    orientation = TurnToFace(position, screenCenter, orientation,
        turnToCenterSpeed);
}

void Prototype1::UpdateTestObjects(float elapsedTime)
{
    for (size_t i = 0; i < objects.size(); i++)
    {
        float distanceFromCat = Vector2::Distance(objects[i]->position, blackTestObject->position);
        if (distanceFromCat > ObjectEvadeDistance + ObjectHysteresis)
        {
            objects[i]->state = AiState::Wander;
        }
        else if (distanceFromCat < ObjectEvadeDistance - ObjectHysteresis)
        {
            objects[i]->state = AiState::Evading;
        }

        objects[i]->speed = 0.0f;

        if (objects[i]->state == AiState::Evading)
        {
            // Turn to point in a straight line away from the chaser
            Vector2 seekPosition = 2 * objects[i]->position - blackTestObject->position;

            // Use the TurnToFace function, which we introduced in the AI Series 1:
            // Aiming sample, to turn the object towards the seekPosition. Now when
            // the object moves forward, it'll be trying to move in a straight line
            // away from the cat.
            objects[i]->orientation = TurnToFace(objects[i]->position, seekPosition,
                objects[i]->orientation, ObjectTurnSpeed);

            // set current object Speed to Max object Speed - the object should run as fast
            // as it can.
            objects[i]->speed = MaxObjectSpeed;
        }
        else
        {
            // if the object isn't trying to evade the cat, it should just meander
            // around the screen. we'll use the Wander function, which the object and
            // tank share, to accomplish this. object Wander Direction and
            // object Orientation are passed by ref so that the wander function can
            // modify them. for more information on ref parameters, see
            // http://msdn2.microsoft.com/en-us/library/14akc2c7(VS.80).aspx
            Wander(objects[i]->position, objects[i]->wanderDirection, objects[i]->orientation,
                ObjectTurnSpeed);

            // if the object is wandering, it should only move at 25% of its maximum
            // speed. 
            objects[i]->speed = .25f * MaxObjectSpeed;
        }

        // The final step is to move the object forward based on its current
        // orientation. First, we construct a "heading" vector from the orientation
        // angle. To do this, we'll use Cosine and Sine to tell us the x and y
        // components of the heading vector. See the accompanying doc for more
        // information.
        Vector2 heading((float)cos(objects[i]->orientation), (float)sin(objects[i]->orientation));

        // by multiplying the heading and speed, we can get a velocity vector. the
        // velocity vector is then added to the object's current position, moving him
        // forward.
        objects[i]->position += heading * objects[i]->speed;
    }
}

void Prototype1::UpdateGreenTestObject(float elapsedTime)
{
    greenTestObject->state = AiState::Wander;
    // if the object isn't trying to evade the cat, it should just meander
    // around the screen. we'll use the Wander function, which the object and
    // tank share, to accomplish this. oject Wander Direction and
    // object Orientation are passed by ref so that the wander function can
    // modify them. for more information on ref parameters, see
    // http://msdn2.microsoft.com/en-us/library/14akc2c7(VS.80).aspx
    Wander(greenTestObject->position, greenTestObject->wanderDirection, greenTestObject->orientation,
        ObjectTurnSpeed);

    // if the object is wandering, it should only move at 25% of its maximum
    // speed. 
    greenTestObject->speed = .25f * MaxObjectSpeed;

    // The final step is to move the object forward based on its current
    // orientation. First, we construct a "heading" vector from the orientation
    // angle. To do this, we'll use Cosine and Sine to tell us the x and y
    // components of the heading vector. See the accompanying doc for more
    // information.
    Vector2 heading((float)cos(greenTestObject->orientation), (float)sin(greenTestObject->orientation));

    // by multiplying the heading and speed, we can get a velocity vector. the
    // velocity vector is then added to the object's current position, moving him
    // forward.
    greenTestObject->position += heading * greenTestObject->speed;
}

void Prototype1::OnUpdate(float elapsedTime)
{
    UNREFERENCED_PARAMETER(elapsedTime);

    UpdateBlackTestObject(elapsedTime);
    UpdateTestObjects(elapsedTime);
    UpdateGreenTestObject(elapsedTime);

    blackTestObject->position = ClampToViewport(blackTestObject->position);
    greenTestObject->position = ClampToViewport(greenTestObject->position);

    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->position = ClampToViewport(objects[i]->position);
    }
}

void Prototype1::OnDraw()
{
    DrawSpriteWithOrigin(squareTexture, blackTestObject->position.x, blackTestObject->position.y,0,1, Colors::Yellow);
    DrawSpriteWithOrigin(arrowTexture, blackTestObject->position.x, blackTestObject->position.y, blackTestObject->orientation, 1,Colors::Black);
    DrawSpriteWithOrigin(squareTexture, greenTestObject->position.x, greenTestObject->position.y,0,1, Colors::Yellow);
    DrawSpriteWithOrigin(arrowTexture, greenTestObject->position.x, greenTestObject->position.y, greenTestObject->orientation, 1,Colors::Green);
    for (size_t i = 0; i < objects.size(); i++)
    {
        DrawSpriteWithOrigin(squareTexture, objects[i]->position.x, objects[i]->position.y,0,1, Colors::Yellow);
        DrawSpriteWithOrigin(arrowTexture, objects[i]->position.x, objects[i]->position.y, objects[i]->orientation);
    }
}