// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game {
  public class TextAnimation : Entity
  {
    private TextComponent textC;
    public float maxKerning = 0.5f;
    public float speed = 18.0f;
    private float increment = 0.0f;

    void OnCreate()
    {
      textC = GetComponent<TextComponent>();
    }
    void OnUpdate(float deltaTime)
    {
      if(textC != null)
      {
        if(textC.Kerning >= maxKerning)
          increment = -1;

        if(textC.Kerning <= 0.0f)
          increment = 1;

        textC.Kerning += increment * (0.01f * speed * deltaTime);
      }
    }
    
  }
}
