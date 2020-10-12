#pragma once
#include <BWAPI.h>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <chrono>

class CameraModule
{
public:
  int scrWidth;
  int scrHeight;
  int watchScoutWorkerUntil;

  int lastMovedPriority;
  BWAPI::Position lastMovedPosition;
  BWAPI::Position currentCameraPosition;
  BWAPI::Position cameraFocusPosition;
  BWAPI::Unit cameraFocusUnit;
  bool followUnit;

  CameraModule();
  void onStart(BWAPI::Position startPos, int screenWidth, int screenHeight);
  void onFrame();
  bool isNearStartLocation(BWAPI::Player player, BWAPI::Position pos);
  bool isNearOwnStartLocation(BWAPI::Player player, BWAPI::Position pos);
  bool isArmyUnit(BWAPI::Unit unit);
  bool shouldMoveCamera(int priority);
  void moveCamera(BWAPI::Position pos, int priority);
  void moveCamera(BWAPI::Unit unit, int priority);
  void moveCameraIsUnderAttack();
  void moveCameraIsAttacking();
  void moveCameraScoutWorker();
  void moveCameraFallingNuke();
  void moveCameraNukeDetect(BWAPI::Position target);
  void moveCameraDrop();
  void moveCameraArmy();
  void moveCameraUnitCreated(BWAPI::Unit unit);
  void updateCameraPosition();

private:
  std::chrono::steady_clock clock;
  std::chrono::time_point<std::chrono::steady_clock> lastMoved;
  std::chrono::duration<int> cameraMoveTime;
  std::chrono::duration<int> cameraMoveTimeMin;
  int localSpeed;
};
