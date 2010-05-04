#include "shared.h"

namespace render
{

namespace low_level
{

namespace opengl
{

//инициализация единичной матрицы
void identity_matrix (Matrix4f m)
{
  for (size_t i=0; i<4; i++)
    for (size_t j=0; j<4; j++)
      m [i][j] = i == j ? 1.0f : 0.0f;
}

//умножение матриц (result = a * b)
void mult_matrix (const Matrix4f a, const Matrix4f b, Matrix4f result)
{
  for (int i=0; i<4; i++)
  {
    for (int j=0; j<4; j++)
    {
      float s = 0.0f;

      for (int k=0; k<4; k++)
        s += a [i][k] * b [k][j];

      result [i][j] = s;
    }
  }
}

//транспонирование матрицы
void transpose_matrix (const Matrix4f source, Matrix4f destination)
{
  for (size_t i=0; i<4; i++)
    for (size_t j=0; j<4; j++)
      destination [i][j] = source [j][i];
}

}

}

}
