#include "core.h"

#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>

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

class TrajectoryBuilder
{
  public:
    TrajectoryBuilder (const ModelData& in_model_data, size_t iterations_count) : model_data (in_model_data) 
    {
      MaxSize = 900000; //?????

      CurSize = elmnts = 0;

      mlen = model_data.len;

      nit = 1;

      if (sqr (model_data.nu1) + sqr (model_data.nu3) > 1)
      {
        model_data.nu3 = sqrt (1.0 - sqr (model_data.nu1));
      }
      
      model_data.nu2 = sqrt (1.0 - sqr (model_data.nu1) - sqr (model_data.nu3));
      
      mlen = model_data.len;      
      dm   = model_data.dm;
      
      qq1 = model_data.nu1;
      qq3 = model_data.nu3;

      alf = 0.2;

      ind = 200;

      point3d.resize (MaxSize);

      Array_y_z ();

/*      float angle = 0;

      for (size_t i = 0; i < 1000; i++, angle += 0.001)
      {
        model_data.nu1 = sin (angle);
        model_data.nu3 = sin (angle);

        if (sqr (model_data.nu1) + sqr (model_data.nu3) > 1)
        {
          model_data.nu3 = sqrtf (1 - sqr (model_data.nu1));
        }

        model_data.nu2 = sqrtf (1 - sqr (model_data.nu1) - sqr (model_data.nu3));

        qq1 = model_data.nu1;
        qq3 = model_data.nu3;

        DoIterEx ();
      }*/

      size_t print_step = iterations_count / 10000;

      if (!print_step)
        print_step++;

      for (size_t i = 0; i < iterations_count; i++)
      {
        if (!(i % print_step))
          printf ("\rProgress: %.2f%%", (float)i / iterations_count * 100.f);

        DoIterEx ();
      }
      printf ("\rProgress: 100.00%%\n");
//      Section3D (200, 1.02);
    }  

    void Convert (DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
    {
      out_vertices.reserve (CurSize * 2);

      size_t first = 0;

      for (int i=0; i < CurSize; i++)
      {
        DrawVertex   line_start, line_end;

//        if ((sqr (point3d[i].pnts2[0]) + sqr (point3d[i].pnts2[1]) + sqr (point3d[i].pnts2[2])) > 2.f)
//          continue;
         
        Point3D& point = point3d [i];
        float    normal [3], normal_length = 0.0f;
        
        for (size_t j=0; j<3; j++)
        {
//          normal [j]     = point.pnts [j] - point.pnts2 [j];
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
      elmnts %= MaxSize; //??????

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

        memcpy (&point3d[elmnts].pnts,  dd,  sizeof (dd));
        memcpy (&point3d[elmnts].pnts2, dd2, sizeof (dd2));
        memcpy (&point3d[elmnts].rgb,   dot_color,  sizeof (dot_color));

        point3d[elmnts].side = koef > 1;
        point3d[elmnts].nu1  = model_data.nu1;
        point3d[elmnts].nu3  = model_data.nu3;

        point3d[elmnts].envelope_side = ii > 0;
        
        if (MaxSize > CurSize)
        {
          CurSize++;
        }

        elmnts++;    
        
        memcpy (&point3d[elmnts].pnts,  dd,  sizeof (dd));
        memcpy (&point3d[elmnts].pnts2, dd2, sizeof (dd2));
        memcpy (&point3d[elmnts].rgb,   dot_color,  sizeof (dot_color));

        point3d[elmnts].pnts[1]  *= -1;
        point3d[elmnts].pnts2[1] *= -1;

        if (sign)
          point3d[elmnts].side = koef < 1;
        else
          point3d[elmnts].side = koef > 1;

        point3d[elmnts].nu1  = model_data.nu1;
        point3d[elmnts].nu3  = model_data.nu3;
        
        point3d[elmnts].envelope_side = ii > 0;
        
        if (MaxSize > CurSize)
        {
          CurSize++;
        }
        
        elmnts++; 
      }         
    }
    
    void q_starting (double *vst, int nd, int nst, int pr)
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

    void derivs (double x,double *v,double *dvdx)
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

    void rk4 (double y[], double dydx[], int n, double x, double h, double yout[])
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

    void rkd (double *vstart, int nd, double x1, double x2, int nstep)
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

    void DoIter ()
    {
      ind += 2;

      if (ind > 201)
      {
        ind = 0;

        Array_y_z ();

        if (nit < 1000)
        {
          nit++;
        }
      }
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
    struct Point3D
    {
      float   pnts[3];
      float   pnts2[3];
      float   nrmls[3];
      double  rgb[3],nu1,nu3;
      bool    side;
      bool    envelope_side;
    };

  private:    
    ModelData            model_data;
    double               xx[201], y[7][201], z[3][201];
    size_t               elmnts;
    int                  MaxSize, CurSize;
    int                  ind;
    int                  nit;
    double               dm;
    double               mlen;
    double               qq1, qq3;
    double               alf;
    std::vector<Point3D> point3d;
};

/*
    Загрузка модели из файла
*/

void LoadModelData (const char* file_name, ModelData& model_data, double nu1, double nu2, double nu3)
{
  FILE* file = fopen (file_name, "rt");
  
  if (!file)
  {
    printf ("File '%s' not found\n", file_name);
    exit (1);
  }

  if (fscanf (file, "A=%lf, B=%lf, C=%lf, h=%lf, g=%lf, nu1=%lf, nu3=%lf, mx=%lf, my=%lf, mz=%lf, ini=%d",
              &model_data.A, &model_data.B, &model_data.C, &model_data.h,&model_data.g, &model_data.nu1,
              &model_data.nu3, &model_data.mx, &model_data.my,&model_data.mz, &model_data.ini) != 11)
  {
    printf ("Error at read model data\n");
    exit (1);
  }

  model_data.nu1 = nu1;
  model_data.nu2 = nu2;
  model_data.nu3 = nu3;

  model_data.len = 10.1f;
  model_data.dm  = 0.6f;
}

/*
    Построение мега-поверхности
*/

void BuildTrajectory (const ModelData& model_data, size_t iterations_count, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
{
  TrajectoryBuilder builder (model_data, iterations_count);
  
  builder.Convert (out_vertices, out_primitives);
}
