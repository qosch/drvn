#include "boards/devboard/board.hpp"

int main() {
  if (drvn::board::Init() != drvn::board::Status::kOk) {
    drvn::board::IndicateFailure();
  }
  for (;;) {
  }
}
