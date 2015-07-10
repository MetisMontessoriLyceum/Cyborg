enum direction {
  FAST_LEFT,
  LEFT,
  FAST_RIGHT,
  RIGHT
};



void init () {

  SetSensor(S4, SENSOR_LOWSPEED);

  // Tell the SMUX to start scanning its ports.
  // this takes 500ms
  if (!HTSMUXscanPorts(S4)) {
    // Scan failed, handle the error
    TextOut(0, LCD_LINE1, "Scan failed!");
    Wait(1000);
    StopAllTasks();
  }


  SetSensorType(IN_1, SENSOR_TYPE_LIGHT_ACTIVE);
  SetSensorType(IN_2, SENSOR_TYPE_LOWSPEED);
  SetSensorType(IN_3, SENSOR_TYPE_LOWSPEED);
  if (!smuxSetSensorLegoLight(msensor_S4_1, true)) {
    TextOut(0, LCD_LINE1, "smux sens 1 failed!");
    Wait(1000);
    StopAllTasks();
  }
  if (!smuxSetSensorLegoLight(msensor_S4_2, true)) {
    TextOut(0, LCD_LINE1, "smux sens 2 failed!");
    Wait(1000);
    StopAllTasks();
  }
  if (!smuxSetSensorLegoLight(msensor_S4_3, true)) {
    TextOut(0, LCD_LINE1, "smux sens 3 failed!");
    Wait(1000);
    StopAllTasks();
  }
  if (!smuxSetSensorLegoLight(msensor_S4_4, true)) {
    TextOut(0, LCD_LINE1, "smux sens 4 failed!");
    Wait(1000);
    StopAllTasks();
  }
  TextOut(0, LCD_LINE1, "ready...");
  until(ButtonPressed(BTNCENTER,false));
  ClearScreen();

}

int sens (int num) {
  int ret;
  switch (num) {
    case -1:
      ret = SensorUS(IN_3);
      break;
    case 0:
      ret = SensorUS(IN_2);
      break;
    case 1:
      ret = smuxSensorLegoLightNorm(msensor_S4_1);
      break;
    case 2:
      ret = smuxSensorLegoLightNorm(msensor_S4_2);
      break;
    case 3:
      ret = Sensor(IN_1);
      break;
    case 4:
      ret = smuxSensorLegoLightNorm(msensor_S4_4);
      break;
    case 5:
      ret = smuxSensorLegoLightNorm(msensor_S4_3);
      break;
    default:
      ClearScreen();
      TextOut(0, LCD_LINE1, "Error:");
      TextOut(0, LCD_LINE2, "num can only be -1 to 5");
      Wait(1000);
  }
  return ret;
}

void updateActive () {
  if (sens(1) < 35) {
    sen1 = true;
  }
  else {
    sen1 = false;
  }

  if (sens(2) < 40) {
    sen2 = true;
  }
  else {
    sen2 = false;
  }
  if (sens(3) < 700) {
    sen3 = true;
  }
  else {
    sen3 = false;
  }

  if (sens(4) < 40) {
    sen4 = true;
  }
  else {
    sen4 = false;
  }

  if (sens(5) < 35) {
    sen5 = true;
  }
  else {
    sen5 = false;
  }
}




void Move (int motorL, int motorR) {
  if (motorL == motorR && motorL != 0) {
    OnFwdReg(OUT_AC,motorL,OUT_REGMODE_IDLE);
  }
  else {
    if (motorL == 0) {
      Off(OUT_A);
    }
    else if (motorL >= 0) {
      OnFwdReg(OUT_A,motorL,OUT_REGMODE_IDLE);
    }
    else {
      OnRevReg(OUT_A,0-motorL,OUT_REGMODE_IDLE);
    }

    if (motorR == 0) {
      Off(OUT_C);
    }
    else if (motorR >= 0) {
      OnFwdReg(OUT_C,motorR,OUT_REGMODE_IDLE);
    }
    else {
      OnRevReg(OUT_C,0-motorR,OUT_REGMODE_IDLE);
    }
  }

}

void WaitForLine (direction d) {
  long OldTime = CurrentTick();
  while (true) {
    if (sens(3) < 600)  {
      break;
    }
    else if (CurrentTick() > OldTime + 1500) {
      switch (d) {
        case FAST_LEFT:
          TextOut(0, LCD_LINE8, "fast left!");
          Move(-75,100);
          Wait(1500);
          ClearLine(LCD_LINE8);
          break;

        case LEFT:
          TextOut(0, LCD_LINE8, "left!");
          Move(-75,75);
          Wait(1500);
          ClearLine(LCD_LINE8);
          break;
        case FAST_RIGHT:
          TextOut(0, LCD_LINE8, "fast right!");
          Move(100,-75);
          Wait(1500);
          ClearLine(LCD_LINE8);
          break;
        case RIGHT:
          TextOut(0, LCD_LINE8, "right!");
          Move(75,-75);
          Wait(1500);
          ClearLine(LCD_LINE8);
          break;
      }
      break;
    }
  }
}

void Ramp () {
  int RampStartTick = CurrentTick();
  while (true) {
    updateActive();
    ClearLine(LCD_LINE2);
    if (sen2) {
      TextOut(0, LCD_LINE2, "left");
      Move(80,100);
    }
    else if (sen4) {
      TextOut(0, LCD_LINE2, "right");
      Move(100,60);
    }
    else {
      TextOut(0, LCD_LINE2, "forward");
      OnFwdReg(OUT_AC,75,OUT_REGMODE_IDLE);
      rotaOLD = rotaNEW;
      rotaNEW = MotorRotationCount(OUT_A);
      tickOLD = tickNEW;
      tickNEW = CurrentTick();

      if (sens(0) < 11) {
        ClearScreen();
        TextOut(1, LCD_LINE1, "Normal");
        Move(-75,-75);
        Wait(600);
        if (CurrentTick() > RampStartTick + 4000) {
          switchTimeOut = CurrentTick();
        }
        break;
      }
      else if (!(rotaNEW - rotaOLD < 4 *( tickNEW - tickOLD ) / 10) )  {
        ClearScreen();
        TextOut(1, LCD_LINE1, "Normal");
        if (CurrentTick() > RampStartTick + 4000) {
          switchTimeOut = CurrentTick();
        }
        break;
      }

    }
  }
}
