#pragma once

#include <Siv3D.hpp>

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