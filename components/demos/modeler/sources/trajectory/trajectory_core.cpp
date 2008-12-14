#include "core.h"

#include <stdio.h>

#include <list>
#include <vector>
#include <algorithm>

const float POINT_EQUAL_EPSILON = 0.001f;

template <class T>
inline T sqr (T value)
{
  return value * value;
}

void hls2rgb3f (double h, double l, double s, double* rgb, int hls)
{
  int    c;
  double frac, diff, mini, maxi;
  double *r, *g, *b;

  r = rgb;
  g = rgb + 1;
  b = rgb + 2;
  h = fmod (h, 360.0);

  if (hls)
  {
    h /= 60.0;
  }
  else
  {
    if      (h < 120) h = h / 120.0;            /* 0..1 Rot..(Orange)..Gelb */
    else if (h < 180) h = h / 60.0 - 1.0;       /* 1..2 Gelb..Gruen */
    else if (h < 240) h = h / 30.0 - 4.0;       /* 2..4 Gruen..Blaugruen..Blau*/
    else              h = h / 60.0;             /* 4..6 Blau..Purpur..Rot */
  }

  c    = (int)h;
  frac = h - c;

  if (l <= 0.5) maxi = l * (1 + s);
  else          maxi = l + s - l * s;

  mini = 2 * l - maxi;
  diff = maxi - mini;

  if (s == 0)                                       /* grau */
  {
    *r = *g = *b = (double)l;
  }
  else
  {
    switch (c)
    {
      case 0:
        *r = (double)maxi;
        *g = (double)(mini + frac * diff);
        *b = (double)mini;
        break;
      case 1:
        *r = (double)(mini + (1.0 - frac) * diff);
        *g = (double)maxi;
        *b = (double)mini;
        break;
      case 2:
        *r = (double)mini;
        *g = (double)maxi;
        *b = (double)(mini + frac * diff);
        break;
      case 3:
        *r = (double)mini;
        *g = (double)(mini +(1.0 - frac) * diff);
        *b = (double)maxi;
        break;
      case 4:
        *r = (double)(mini + frac * diff);
        *g = (double)mini;
        *b = (double)maxi;
        break;
      case 5:
        *r = (double)maxi;
        *g = (double)mini;
        *b = (double)(mini + (1.0 - frac) * diff);
        break;
    }
  }
}

double* vector (int nl, int nh)
{
  double* v = (double*)malloc ((unsigned)(nh - nl + 1) * sizeof (double));

  if (!v)
  {
    printf ("allocation failure in vector(%d,%d)", nl, nh);
    abort  ();
  }

  return v - nl;
}

void free_vector(double *v,int nl,int)
{
  free ((char*)(v + nl));
}

bool float_equal (float value1, float value2, float eps)
{
  return (fabs (value1 - value2) < eps);
}

struct Point3D
{
  float  pnts[3];
  float  pnts2[3];
  float  nrmls[3];
  double rgb[3],nu1,nu3;
  bool   side;
  bool   envelope_side;
};

struct HashSet
{
  HashSet (size_t hash_table_size)
    : hash_table (hash_table_size), size (0)
  {
    if (!hash_table_size)
      hash_table.resize (1);

    hash_multiplier = (size_t)pow (10.f, (int)log10 ((float)hash_table_size) - 1);

    if (!hash_multiplier)
      hash_multiplier = 10;
  }

  void Insert (Point3D* point)
  {
    size_t point_hash = (size_t)fabs (point->pnts2[0] * hash_multiplier + point->pnts2[1] * hash_multiplier + point->pnts2[2] * hash_multiplier);

    point_hash %= hash_table.size ();

    bool point_exist = false;

    for (PointList::iterator iter = hash_table[point_hash].begin (), end = hash_table[point_hash].end (); iter != end; ++iter)
    {
      if (Point3dEqualPredicate (point, *iter))
      {
        point_exist = true;
        break;
      }
    }

    if (point_exist)
      return;

    hash_table[point_hash].push_front (point);
    size++;
  }

  size_t Size ()
  {
    return size;
  }

  bool Point3dEqualPredicate (Point3D* point1, Point3D* point2)
  {
    return float_equal (point1->pnts2[0], point2->pnts2[0], POINT_EQUAL_EPSILON) &&
           float_equal (point1->pnts2[1], point2->pnts2[1], POINT_EQUAL_EPSILON) &&
           float_equal (point1->pnts2[2], point2->pnts2[2], POINT_EQUAL_EPSILON) &&
           (point1->envelope_side == point2->envelope_side);
  }

  typedef std::list<Point3D*>    PointList;
  typedef std::vector<PointList> PointListArray;

  PointListArray hash_table;
  size_t         size;
  size_t         hash_multiplier;
};

class TrajectoryBuilder
{
  public:
    TrajectoryBuilder (const ModelData& in_model_data, double in_nu1, double in_nu2, double in_nu3, size_t vertices_count)
      : model_data (in_model_data),
        nu1 (in_nu1),
        nu2 (in_nu2),
        nu3 (in_nu3)
    {
      current_size = 0;

      mlen = model_data.len;

      nit = 1;

      if (sqr (nu1) + sqr (nu3) > 1)
      {
        nu3 = sqrt (1.0 - sqr (nu1));
      }

      nu2 = sqrt (1.0 - sqr (nu1) - sqr (nu3));

      mlen = model_data.len;
      dm   = model_data.dm;

      qq1 = nu1;
      qq3 = nu3;

      alf = 0.2;

      ind = 200;

      point3d.resize (vertices_count + 8);

      Array_y_z ();

      size_t print_step = vertices_count / 100;

      if (!print_step)
        print_step++;

      size_t current_points = -1;
      size_t constant_size_interval = 0;

      for (size_t i = 0; current_size < vertices_count; i++)
      {
        if (!(i % print_step))
          printf ("\rProgress: %.2f%%", (float)current_size / vertices_count * 100.f);

        DoIterEx ();

        if (current_points != current_size)
        {
          current_points         = current_size;
          constant_size_interval = 0;
        }
        else
        {
          constant_size_interval++;
          if (constant_size_interval > 1000) //???? непон€тное место
          {
            printf ("\nTrajectory not grow, possibly invalid input data\n");
            return;
          }
        }
      }
      printf ("\rProgress: 100.00%%\n");
    }

    void Convert (DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
    {
      out_vertices.reserve (current_size * 2);

      for (size_t i=0; i < current_size; i++)
      {
        DrawVertex   line_start, line_end;

        Point3D& point = point3d [i];
        float    normal [3], normal_length = 0.0f;

        for (size_t j=0; j<3; j++)
        {
          normal [j]     = point.pnts2 [j];
          normal_length += sqr (normal [j]);
        }

        normal_length = sqrt (normal_length);

        for (size_t j=0; j<3; j++)
          normal [j] /= normal_length;

        line_start.position.x = point.pnts2 [0];
        line_start.position.y = point.pnts2 [1];
        line_start.position.z = point.pnts2 [2];
        line_start.color.r    = 0.2f;
        line_start.color.g    = 0.2f;
        line_start.color.b    = 0.2f;
        line_start.color.a    = 1.f;

        float direction = point.envelope_side ? 1.0f : -1.0f;

        line_end.position.x = line_start.position.x + direction * normal [0] * 0.1f;
        line_end.position.y = line_start.position.y + direction * normal [1] * 0.1f;
        line_end.position.z = line_start.position.z + direction * normal [2] * 0.1f;
        line_end.color.r    = (float)point3d[i].rgb[0];
        line_end.color.g    = (float)point3d[i].rgb[1];
        line_end.color.b    = (float)point3d[i].rgb[2];
        line_end.color.a    = 1.f;

        out_vertices.push_back (line_start);
        out_vertices.push_back (line_end);
      }

      DrawPrimitive line;

      line.type  = PrimitiveType_LineList;
      line.first = 0;
      line.count = out_vertices.size () / 2;

      out_primitives.push_back (line);
    }

    void ConvertUnique (DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
    {
      HashSet unique_points (point3d.size () / 10);

      for (size_t i = 0; i < current_size; i++)
      {
        unique_points.Insert (&point3d[i]);
      }

//      printf ("Unique points count is %u\n", unique_points.Size ());

      out_vertices.reserve (unique_points.Size () * 2);

      for (HashSet::PointListArray::iterator iter = unique_points.hash_table.begin (), end = unique_points.hash_table.end (); iter != end; ++iter)
      {
        for (HashSet::PointList::iterator list_iter = iter->begin (), list_end = iter->end (); list_iter != list_end; ++list_iter)
        {
          DrawVertex line_start, line_end;

          Point3D& point = **list_iter;
          float    normal [3], normal_length = 0.0f;

          for (size_t j=0; j<3; j++)
          {
            normal [j]     = point.pnts2 [j];
            normal_length += sqr (normal [j]);
          }

          normal_length = sqrt (normal_length);

          for (size_t j=0; j<3; j++)
            normal [j] /= normal_length;

          line_start.position.x = point.pnts2 [0];
          line_start.position.y = point.pnts2 [1];
          line_start.position.z = point.pnts2 [2];
          line_start.color.r    = 0.2f;
          line_start.color.g    = 0.2f;
          line_start.color.b    = 0.2f;
          line_start.color.a    = 1.f;

          float direction = point.envelope_side ? 1.0f : -1.0f;

          line_end.position.x = line_start.position.x + direction * normal [0] * 0.1f;
          line_end.position.y = line_start.position.y + direction * normal [1] * 0.1f;
          line_end.position.z = line_start.position.z + direction * normal [2] * 0.1f;
          line_end.color.r    = (float)point.rgb[0];
          line_end.color.g    = (float)point.rgb[1];
          line_end.color.b    = (float)point.rgb[2];
          line_end.color.a    = 1.f;

          out_vertices.push_back (line_start);
          out_vertices.push_back (line_end);
        }
      }

      DrawPrimitive line;

      line.type  = PrimitiveType_LineList;
      line.first = 0;
      line.count = out_vertices.size () / 2;

      out_primitives.push_back (line);
    }

  private:
    typedef std::vector<Point3D> Point3DArray;

  private:
    double CalculateFaI (int i)
    {
      return model_data.mx * (model_data.B * y[1][i] * y[5][i] - model_data.C * y[2][i] * y[4][i]) +
             model_data.my * (model_data.C * y[2][i] * y[3][i] - model_data.A * y[0][i] * y[5][i]) +
             model_data.mz * (model_data.A * y[0][i] * y[4][i] - model_data.B * y[1][i] * y[3][i]);
    }

    bool ii_change_condition ()
    {
      return CalculateFaI (1) > 0;
    }

    double MegaFormulaWithMaximumInertia (double maximum_moment_of_inertia)
    {
      return 2.0 * maximum_moment_of_inertia * (model_data.h + sqrt (sqr (model_data.mx) + sqr (model_data.my) + sqr (model_data.mz))) - sqr (model_data.g);
    }

    void Section3D (int nst, double koef)
    {
      int    i, ii = 1;
      float  fa = 0, fb = 0;
      double maximum_moment_of_inertia;

      maximum_moment_of_inertia = std::max (model_data.A, std::max (model_data.B, model_data.C));

      if (ii_change_condition ())
      {
        ii = -1;
      }

      for (i = 1; i <= nst; i++)
      {
        BuildPoint (i, fb, fa, ii, maximum_moment_of_inertia, koef, true);
      }

      ii = -1;

      if (ii_change_condition ())
      {
        ii = 1;
      }

      for (i = 1; i <= nst; i++)
      {
        BuildPoint (i, fb, fa, ii, maximum_moment_of_inertia, koef, false);
      }
    }

    void BuildPoint (size_t i, float& fb, float& fa, int& ii, double maximum_moment_of_inertia, double koef, bool sign)
    {
      if (current_size >= point3d.size ())
        return;

      fb = fa;

      fa = (float)CalculateFaI (i);

      if (sign)
      {
        if (ii * fa <= 0)
          return;
      }
      else
      {
        if (ii * fa >= 0)
          return;
      }

      ii =- ii;

      if (fa == fb)
        return;

      bool   build_point_condition;
      double condition_value = y[0][i] * (y[4][i] * (y[3][i] * model_data.my - y[4][i] * model_data.mx) - y[5][i] * (y[5][i] * model_data.mx - y[3][i] * model_data.mz)) +
                               y[1][i] * (y[5][i] * (y[4][i] * model_data.mz - y[5][i] * model_data.my) - y[3][i] * (y[3][i] * model_data.my - y[4][i] * model_data.mx)) +
                               y[2][i] * (y[3][i] * (y[5][i] * model_data.mx - y[3][i] * model_data.mz) - y[4][i] * (y[4][i] * model_data.mz - y[5][i] * model_data.my));

      if (sign)
        build_point_condition = condition_value > 0;
      else
        build_point_condition = condition_value < 0;

      if (build_point_condition && (y[4][i] > 0))
      {
        double m = (sqr (model_data.A) * sqr (y[0][i - 1] * fa - y[0][i] * fb) +
                    sqr (model_data.B) * sqr (y[1][i - 1] * fa - y[1][i] * fb) +
                    sqr (model_data.C) * sqr (y[2][i - 1] * fa - y[2][i] * fb)) / sqr (fa - fb);

        double dot_color[3];
        float  dd[3], dd2[3];

        if (MegaFormulaWithMaximumInertia (maximum_moment_of_inertia) != 0)
        {
          hls2rgb3f (sqrt ((m - sqr (model_data.g)) / MegaFormulaWithMaximumInertia (maximum_moment_of_inertia)) * 360., 0.5, 1.0, dot_color, 1);
        }

        dd[0] = (float)(y[3][i - 1] * fa - y[3][i] * fb) / (fa - fb);
        dd[1] = (float)(y[4][i - 1] * fa - y[4][i] * fb) / (fa - fb);
        dd[2] = (float)(y[5][i - 1] * fa - y[5][i] * fb) / (fa - fb);

        dd2[0] = (float)(dm * (y[0][i - 1] * fa - y[0][i] * fb) / (fa - fb));
        dd2[1] = (float)(dm * (y[1][i - 1] * fa - y[1][i] * fb) / (fa - fb));
        dd2[2] = (float)(dm * (y[2][i - 1] * fa - y[2][i] * fb) / (fa - fb));

        memcpy (&point3d[current_size].pnts,  dd,  sizeof (dd));
        memcpy (&point3d[current_size].pnts2, dd2, sizeof (dd2));
        memcpy (&point3d[current_size].rgb,   dot_color,  sizeof (dot_color));

        point3d[current_size].side = koef > 1;
        point3d[current_size].nu1  = nu1;
        point3d[current_size].nu3  = nu3;

        point3d[current_size].envelope_side = ii > 0;

        current_size++;

        memcpy (&point3d[current_size].pnts,  dd,  sizeof (dd));
        memcpy (&point3d[current_size].pnts2, dd2, sizeof (dd2));
        memcpy (&point3d[current_size].rgb,   dot_color,  sizeof (dot_color));

        point3d[current_size].pnts[1]  *= -1;
        point3d[current_size].pnts2[1] *= -1;

        if (sign)
          point3d[current_size].side = koef < 1;
        else
          point3d[current_size].side = koef > 1;

        point3d[current_size].nu1  = nu1;
        point3d[current_size].nu3  = nu3;

        point3d[current_size].envelope_side = ii > 0;

        current_size++;
      }
    }

    void q_starting (double *vst, int nd, int nst, int pr) // мен€ет vst
    {
      int    i;
      double q1, q2, q3, det;

      if (!pr)
      {
        double u1, u2, u3, v1, v2, v3, vv;

        q1 = qq1;
        q3 = qq3;
        q2 = sqrt (1.0 + 1.0e-20 - sqr (q1) - sqr (q3)); //???

        u1 = q2 * model_data.mz - q3 * model_data.my;
        u2 = q3 * model_data.mx - q1 * model_data.mz;
        u3 = q1 * model_data.my - q2 * model_data.mx;

        v1 = q2 * u3 - q3 * u2;
        v2 = q3 * u1 - q1 * u3;
        v3 = q1 * u2 - q2 * u1;

        vv = sqr (v1) / model_data.A + sqr (v2) / model_data.B + sqr (v3) / model_data.C;

        det = 2.0 * (model_data.h + q1 * model_data.mx + q2 * model_data.my + q3 * model_data.mz) * vv -
              sqr (model_data.g) * (model_data.A * sqr (u1) + model_data.B * sqr (u2) + model_data.C * sqr (u3)) / model_data.A / model_data.B / model_data.C;

                                  //here put +-sqrt

        vst[1] = ((u2 * v3 / model_data.C - u3 * v2 / model_data.B) * model_data.g + model_data.ini * v1 * sqrt (det)) / model_data.A / vv;
        vst[2] = ((u3 * v1 / model_data.A - u1 * v3 / model_data.C) * model_data.g + model_data.ini * v2 * sqrt (det)) / model_data.B / vv;
        vst[3] = ((u1 * v2 / model_data.B - u2 * v1 / model_data.A) * model_data.g + model_data.ini * v3 * sqrt (det)) / model_data.C / vv;
        vst[4] = q1;
        vst[5] = q2;
        vst[6] = q3;
        vst[7] = alf;
      }
      else
      {
        for (i = 1; i <= nd; i++)
        {
          vst[i] = y[i - 1][nst];
        }
      }
    }

    void derivs (double x,double *v,double *dvdx) //заполн€ет dvdx
    {
      double d1,d2;

      dvdx[1] = (model_data.B - model_data.C) / model_data.A * v[2] * v[3] + (model_data.my * v[6] - model_data.mz * v[5]) / model_data.A;
      dvdx[2] = (model_data.C - model_data.A) / model_data.B * v[3] * v[1] + (model_data.mz * v[4] - model_data.mx * v[6]) / model_data.B;
      dvdx[3] = (model_data.A - model_data.B) / model_data.C * v[1] * v[2] + (model_data.mx * v[5] - model_data.my * v[4]) / model_data.C;
      dvdx[4] = v[3] * v[5] - v[2] * v[6];
      dvdx[5] = v[1] * v[6] - v[3] * v[4];
      dvdx[6] = v[2] * v[4] - v[1] * v[5];

      d1 = v[1] * v[4] + v[2] * v[5] + v[3] * v[6];
      d2 = v[1] * v[1] + v[2] * v[2] + v[3] * v[3] - d1 * d1;

      dvdx[7] = dvdx[1] * dvdx[4] + dvdx[2] * dvdx[5] + dvdx[3] * dvdx[6];
      dvdx[7] = dvdx[7] / d2;
    }

    void rk4 (double y[], double dydx[], int n, double x, double h, double yout[]) //заполн€ет yout
    {
      int    i;
      double xh, hh, h6, *dym, *dyt, *yt;

      dym = vector (1,n);
      dyt = vector (1,n);
      yt  = vector (1,n);

      hh = h * 0.5;
      h6 = h / 6.0;
      xh = x + hh;

      for (i = 1; i <= n; i++)
        yt[i] = y[i] + hh * dydx[i];

      derivs (xh, yt, dyt);

      for (i = 1; i <= n; i++)
        yt[i] = y[i] + hh * dyt[i];

      derivs (xh, yt, dym);

      for (i = 1; i <= n; i++)
      {
        yt[i] = y[i] + h * dym[i];

        dym[i] += dyt[i];
      }

      derivs (x + h, yt, dyt);

      for (i = 1; i <= n; i++)
        yout[i] = y[i] + h6 * (dydx[i] + dyt[i] + 2.0 * dym[i]);

      free_vector (yt,  1, n);
      free_vector (dyt, 1, n);
      free_vector (dym, 1, n);
    }

    void rkd (double *vstart, int nd, double x1, double x2, int nstep) //мен€ет y, z, xx
    {
      int    i, j;
      double x, sh, d;
      double *v, *vout, *dv;

      v    = vector (1, nd);
      vout = vector (1, nd);
      dv   = vector (1, nd);

      for (i = 1; i <= nd; i++)
      {
        v[i]        = vstart[i];
        y[i - 1][0] = v[i];
      }

      z[0][0] = v[1] * v[4] + v[2] * v[5] + v[3] * v[6];

      d = sqrt (sqr (v[1]) + sqr (v[2]) + sqr (v[3]) - sqr (z[0][0]));

      z[1][0] = d * cos (v[7]);
      z[2][0] = d * sin (v[7]);

      xx[0] = x1;
      x     = x1;
      sh    = (x2 - x1) / nstep;

      for (j = 1; j <= nstep; j++)
      {
        derivs (x, v, dv);
        rk4    (v, dv, nd, x, sh, vout);

        x = x + sh;

        xx[j] = x;

        for (i = 1; i <= nd; i++)
        {
          v[i]        = vout[i];
          y[i - 1][j] = v[i];
        }

        z[0][j] = v[1] * v[4] + v[2] * v[5] + v[3] * v[6];

        d = sqrt (sqr (v[1]) + sqr (v[2]) + sqr (v[3]) - sqr (z[0][j]));

        z[1][j] = d * cos (v[7]);
        z[2][j] = d * sin (v[7]);
      }

      free_vector (dv,   1, nd);
      free_vector (vout, 1, nd);
      free_vector (v,    1, nd);
    }

    void Array_y_z ()
    {
      double x1 = 0.0, x2, dx = mlen, *vstart;

      int nstep = 200, ndim = 10;

      vstart = vector (1, ndim);

      x2 = x1 + dx;

      q_starting (vstart, ndim, nstep, nit - 1);
      rkd        (vstart, ndim, x1, x2, nstep);

      x1 += dx;

      free_vector (vstart, 1, ndim);
    }

    void DoIterEx ()
    {
      ind += 2;

      if (ind > 201)
      {
        ind = 0;

        if (nit < 1000)
        {
          nit++;
        }
      }

      Array_y_z ();
      Section3D (200, 1.02);
    }

  private:
    ModelData            model_data;
    double               nu1, nu2, nu3;
    double               xx[201], y[7][201], z[3][201];
    size_t               current_size;                      //текущее количество рассчитанных точек
    int                  ind;
    int                  nit;
    double               dm;
    double               mlen;
    double               qq1, qq3;
    double               alf;
    Point3DArray         point3d;
};

/*
    «агрузка модели из файла
*/

void LoadModelData (const char* file_name, ModelData& model_data)
{
  FILE* file = fopen (file_name, "rt");

  if (!file)
  {
    printf ("File '%s' not found\n", file_name);
    exit (1);
  }

  if (fscanf (file, "A=%lf, B=%lf, C=%lf, h=%lf, g=%lf, mx=%lf, my=%lf, mz=%lf, ini=%d",
              &model_data.A, &model_data.B, &model_data.C, &model_data.h,&model_data.g,
              &model_data.mx, &model_data.my,&model_data.mz, &model_data.ini) != 9)
  {
    printf ("Error at read model data\n");
    exit (1);
  }

  model_data.len = 10.1f;
  model_data.dm  = 0.6f;
}

/*
    ѕостроение мега-поверхности
*/

void BuildTrajectory (const ModelData& model_data, double nu1, double nu2, double nu3, size_t vertices_count, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
{
  TrajectoryBuilder builder (model_data, nu1, nu2, nu3, vertices_count);

  builder.ConvertUnique (out_vertices, out_primitives);

//  builder.Convert (out_vertices, out_primitives);
}
