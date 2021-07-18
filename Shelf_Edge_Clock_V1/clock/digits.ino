//called to set the each segment
void displayNumber(int digitToDisplay, int offsetBy, int colourToUse){
    switch (digitToDisplay){
    case 0:
    digitZero(offsetBy,colourToUse);
      break;
    case 1:
      digitOne(offsetBy,colourToUse);
      break;
    case 2:
    digitTwo(offsetBy,colourToUse);
      break;
    case 3:
    digitThree(offsetBy,colourToUse);
      break;
    case 4:
    digitFour(offsetBy,colourToUse);
      break;
    case 5:
    digitFive(offsetBy,colourToUse);
      break;
    case 6:
    digitSix(offsetBy,colourToUse);
      break;
    case 7:
    digitSeven(offsetBy,colourToUse);
      break;
    case 8:
    digitEight(offsetBy,colourToUse);
      break;
    case 9:
    digitNine(offsetBy,colourToUse);
      break;
    default:
     break;
  }
}

void digitZero(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 27); //light segments BAF
    stripClock.fill(colour, (36 + offset), 27);//light segments CDE
}

void digitOne(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 9);
    stripClock.fill(colour, (36 + offset), 9);
}

void digitTwo(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 18);
    stripClock.fill(colour, (27 + offset), 9);
    stripClock.fill(colour, (45 + offset), 18);
}

void digitThree(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 18);
    stripClock.fill(colour, (27 + offset), 27);
}

void digitFour(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 9);
    stripClock.fill(colour, (18 + offset), 27);
}

void digitFive(int offset, int colour){
    stripClock.fill(colour, (9 + offset), 45);
}

void digitSix(int offset, int colour){
    stripClock.fill(colour, (9 + offset), 54);
}

void digitSeven(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 18);
    stripClock.fill(colour, (36 + offset), 9);
}

void digitEight(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 63);
}

void digitNine(int offset, int colour){
    stripClock.fill(colour, (0 + offset), 45);
}
