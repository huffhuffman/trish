#include <Siv3D.hpp>  // OpenSiv3D v0.4.2

/* consts */
constexpr int32 CellSize = 30;
const Vec2 Cell(CellSize, CellSize);
const Vec2 Board(12, 21);
const Vec2 BoardSize = Board * CellSize;

const int32 offsetX = 30;
const int32 offsetY = 100;
const Vec2 offset(offsetX, offsetY);

const int32 InfoFieldX = 440;

const Color Black = Color(2, 17, 27);
const Color Gray = Color(27, 38, 49);
const Color White = Color(252, 252, 252);
const Color LightGreen = Color(0, 149, 165);
const Color Green = Color(28, 124, 84);
const Color Pink = Color(238, 66, 102);
const Color Red = Color(251, 54, 64);
const Color Blue = Color(0, 95, 178);
const Color DarkBlue = Color(31, 86, 115);
const Color LightBlue = Color(7, 135, 255);
const Color LightBlue20 = Color(7, 135, 255, 20);
const Color LightBlue50 = Color(7, 135, 255, 50);

const Color LBlack = Color(0, 0, 0, 20);

/* mino */
struct Mino {
  static inline const Grid<bool> J = {
      {0, 1, 0},
      {0, 1, 0},
      {1, 1, 0},
  };

  static inline const Grid<bool> L = {
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 1},
  };

  static inline const Grid<bool> S = {
      {0, 1, 1},
      {1, 1, 0},
      {0, 0, 0},
  };

  static inline const Grid<bool> Z = {
      {1, 1, 0},
      {0, 1, 1},
      {0, 0, 0},
  };

  static inline const Grid<bool> T = {
      {1, 1, 1},
      {0, 1, 0},
      {0, 0, 0},
  };

  static inline const Grid<bool> I = {
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0},
      {0, 0, 1, 0},
  };

  static inline const Grid<bool> C = {
      {0, 1, 1},
      {0, 1, 0},
      {0, 1, 1},
  };
};

enum class GameState {
  Playing,
  Gameover,
};

/* dynamic vars */
GameState gameState = GameState::Playing;

Grid<int32> g(12, 21);

Grid<bool> currentBlock = Mino::C;

Point currentPoint(4, 0);

double gravityLevel = 0.2;

int64 score = 0;

int64 deleteCount = 0;

bool abilityReady = true;

/* free methods */
void initGrid() {
  for (auto y : step(g.height())) {
    for (auto x : step(g.width())) {
      if (y < 20) {
        g[y][x] = x == 0 || x == 11;
      } else {
        g[y][x] = 1;
      }
    }
  }
}

void drawGrid() {
  for (auto y : step(g.height())) {
    for (auto x : step(g.width())) {
      Color color(Palette::Black);
      Color flameColor(Palette::Black);

      switch (g[y][x]) {
        case 0:
          color = Black;
          flameColor = LightBlue20;
          break;
        case 1:
          color = Gray;
          flameColor = LightBlue50;
          break;
        default:
          color = Blue;
          flameColor = LBlack;
          break;
      }

      Vec2 pos = Vec2(x, y) * Cell + offset;

      RectF(pos, CellSize).draw(color).drawFrame(1, 0, flameColor);
    }
  }
}

void drawMino() {
  for (auto y : step(currentBlock.height())) {
    for (auto x : step(currentBlock.width())) {
      if (currentBlock[y][x]) {
        double xp = offsetX + currentPoint.x * CellSize +
                    static_cast<int32>(x) * CellSize;
        double yp = offsetY + currentPoint.y * CellSize +
                    static_cast<int32>(y) * CellSize;

        RectF(xp, yp, CellSize).draw(Pink).drawFrame(1, 0, Color(0, 0, 0, 20));
      }
    }
  }
}

bool verify(Point nextP) {
  for (auto y : step(currentBlock.height())) {
    for (auto x : step(currentBlock.width())) {
      if (currentBlock[y][x]) {
        Point gridPoint = nextP + Point(x, y);

        if (g[gridPoint.y][gridPoint.x]) {
          return false;
        }
      }
    }
  }

  return true;
}

bool rotate() {
  Grid<bool> tmp = currentBlock;

  for (auto y : step(currentBlock.height())) {
    for (auto x : step(currentBlock.width())) {
      int32 lim = static_cast<int32>(currentBlock.width()) - 1;

      currentBlock[x][lim - y] = tmp[y][x];
    }
  }

  if (!verify(currentPoint)) {
    currentBlock = tmp;
    return false;
  } else {
    return true;
  }
}

void changeMino() {
  currentPoint = Point(4, 0);

  Array<Grid<bool>> minoList = {
      Mino::I, Mino::J, Mino::L, Mino::C, Mino::S, Mino::T, Mino::Z,
  };

  int32 nextMinoIdx = Random<int32>(6);

  currentBlock = minoList[nextMinoIdx];
}

void freezeMino() {
  for (auto y : step(currentBlock.height())) {
    for (auto x : step(currentBlock.width())) {
      if (currentBlock[y][x]) {
        Point gridPoint = currentPoint + Point(x, y);

        g[gridPoint.y][gridPoint.x] = 2;
      }
    }
  }

  abilityReady = std::max(1, 0);
}

bool minoSafeness() {
  for (auto y : step(currentBlock.height())) {
    for (auto x : step(currentBlock.width())) {
      if (currentBlock[y][x]) {
        Point gridPoint = currentPoint + Point(x, y);

        if (g[gridPoint.y][gridPoint.x]) {
          return false;
        }
      }
    }
  }

  return true;
}

void next() {
  freezeMino();
  changeMino();
  if (!minoSafeness()) {
    gameState = GameState::Gameover;
  }
}

void gravity(Stopwatch& gravitywatch) {
  if (gravitywatch.sF() > gravityLevel) {
    if (verify(currentPoint + Point(0, 1))) {
      currentPoint += Point(0, 1);
    }

    gravitywatch.restart();
  }
}

void freeze(Stopwatch& freezewatch, bool forceFreeze = false) {
  if (!verify(currentPoint + Point(0, 1))) {
    if (!freezewatch.isRunning()) {
      freezewatch.restart();
    }
  } else {
    freezewatch.reset();
  }

  if (freezewatch.sF() > 0.5 || forceFreeze) {
    next();
  }
}

void Main() {
  Window::Resize(800, 800);

  initGrid();

  Stopwatch gravitywatch(true);

  Stopwatch msgwatch(true);

  Stopwatch inputwatch(true);

  Stopwatch freezewatch(false);

  const Font font16(16);

  const Font font20(20);

  const Font font30(30);

  String msg = U"";

  Effect effect;

  auto pushMsg = [&msgwatch, &msg](String text) {
    msg = text;
    msgwatch.restart();
  };

  auto showMsg = [&msgwatch, &msg, &font30]() {
    font30(msg).draw(InfoFieldX, offsetY);

    if (msgwatch.sF() > 0.5) {
      msg = U"";
    }
  };

  auto handleInput = [&]() {
    if (KeyRight.pressed() && verify(currentPoint + Point(1, 0))) {
      pushMsg(U"→");

      currentPoint.x++;
      inputwatch.restart();
    }

    if (KeyLeft.pressed() && verify(currentPoint - Point(1, 0))) {
      pushMsg(U"←");

      currentPoint.x--;
      inputwatch.restart();
    }

    if (KeyDown.pressed() && verify(currentPoint + Point(0, 1))) {
      pushMsg(U"↓");

      currentPoint.y++;
      inputwatch.restart();
    }

    if (KeySpace.pressed() && inputwatch.sF() >= 0.2) {
      pushMsg(U"回転");

      if (rotate()) {
        inputwatch.restart();
      } else {
        pushMsg(U"回転不可");
      }
    }

    if (KeyUp.pressed() && abilityReady) {
      pushMsg(U"特殊能力発動");

      changeMino();
      inputwatch.restart();

      abilityReady = false;
    }
  };

  auto deleteRow = [&](int32 row) {
    Grid<int32> nextG = g;

    for (int32 y = 0; y < row; y++) {
      for (int32 x = 0; x < g.width(); x++) {
        nextG[y + 1][x] = g[y][x];
      }
    }

    g = nextG;
  };

  auto checkGrid = [&]() {
    for (int32 y = 0; y < g.height() - 1; y++) {
      bool deletable = true;
      for (int32 x = 0; x < g.width(); x++) {
        if (g[y][x] == 0) {
          deletable = false;
          break;
        }
      }

      if (deletable) {
        deleteRow(y);

        score += y * 2;

        deleteCount++;

        if (deleteCount % 4 == 0 && gravityLevel > 0.06) {
          gravityLevel *= 0.8;
        }

        effect.add([y](double t) {
          const double e = EaseOutExpo(t);

          for (int32 i = 0; i < 10; i++) {
            Circle(offset + Point(15, 15) + Point(1 + i, y) * CellSize,
                   e * CellSize / 2)
                .drawFrame(10.0 * (1.0 - e), White);
          }

          return t < 0.2;
        });
      }
    }
  };

  auto drawInstruction = [&]() {
    font30(U"←↓→: 移動").draw(InfoFieldX, 450);
    font30(U"スペースキー: 回転").draw(InfoFieldX, 500);
    font30(U"↑: 特殊能力").draw(InfoFieldX, 550);
    font16(U"特殊能力はミノの設置に成功すると回復します。")
        .draw(InfoFieldX, 600);
    font16(U"{}秒毎に1セル落下"_fmt(gravityLevel)).draw(InfoFieldX, 700);
  };

  auto drawGameInfo = [&]() {
    font30(U"Score: {}"_fmt(score)).draw(offset.x, 10);
    font20(U"特殊能力: {}"_fmt(abilityReady ? U"発動可" : U"回復待ち"))
        .draw(offset.x, 50);
  };

  while (System::Update()) {
    drawInstruction();

    drawGameInfo();

    checkGrid();

    drawGrid();

    if (Scene::Time() <= 3) {
      int32 countDown = 3 - static_cast<int32>(Scene::Time());
      font30(U"ゲーム開始...{}"_fmt(countDown)).draw(InfoFieldX, offsetY);
      continue;
    }

    drawMino();

    freeze(freezewatch);

    gravity(gravitywatch);

    effect.update();

    if (gameState == GameState::Gameover) {
      font30(U"ゲームオーバー").draw(InfoFieldX, offset.y);
      continue;
    }

    if (inputwatch.sF() > 0.1) {
      handleInput();
    }

    showMsg();
  }
}
