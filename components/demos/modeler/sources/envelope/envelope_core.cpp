#include <algorithm>

#include "core.h"

namespace modeler
{

namespace
{

#pragma pack (1)

const float PI  = 3.1415926f;
const float EPS = 0.001f;

struct Vertex
{
  Vec3d   position;
  Vec3d   dummy;
  Vec3d   normal;
  int     attr;
};

template <class T1, class T2> void copy (const Vec3<T1>& src, Vec3<T2>& dst)
{
  dst.x = static_cast<T2> (src.x);
  dst.y = static_cast<T2> (src.y);
  dst.z = static_cast<T2> (src.z);
}

inline double cos2 (double angle)
{
  double value = cos (angle);

  return value * value;
}

inline double sin2 (double angle)
{
  double value = sin (angle);

  return value * value;
}

int sol3 (double a, double b, double c, double d, double result [3])
{
  double r0, s, t;
  double p0, q0, delta;
  double radi, u, v, rho, phi, phi3, factor;

  r0    = b / a;
  s     = c / a;
  t     = d / a;
  p0    = (3.0 * s - r0 * r0) / 3.0;
  q0    = 2.0* r0 * r0 * r0 / 27.0 - r0 * s / 3.0 + t;
  delta = p0 * p0 * p0 / 27.0 + q0 * q0 / 4.0;

  if (delta > 0.0)
  {
    radi = sqrt (delta) + fabs (q0 / 2.0);
    u    = pow (radi, 1.0 / 3.0);
    v    = -p0 / 3.0 / u;

    if (q0 < 0.0)
    {
      result [0] = (u + v) - r0 / 3.0;
      result [1] = (-(u + v) / 2.0) - r0 / 3.0;   /* Realteil */
      result [2] = sqrt (3.0) * (u -v) / 2.0;  /* Imaginaerteil */
    }
    else
    {
      result [0] =-(u + v) - r0 / 3.0;
      result [1] =((u + v) / 2.0) - r0 / 3.0;
      result [2] =-sqrt (3.0) *(u - v) / 2.0;
    }

    return 1;
  }
  else
  {
    rho = sqrt (-p0 * p0 * p0 / 27.0);

    if (fabs (rho) == 0.0)
    {
      result [0] = -b / a / 3.0;
      result [1] = -b / a / 3.0;
      result [2] = 0.0;

      return 1;
    }

    phi    = acos (-q0 / 2.0 / rho);
    phi3   = phi / 3.0;
    factor = 2.0 * pow (rho, 1.0 / 3.0);

    result [0] = factor * cos (phi3) - r0 / 3.0;
    result [1] = factor * cos (phi3 + 2.0 * PI / 3.0) - r0 / 3.0;
    result [2] = factor * cos (phi3 + 4.0 * PI / 3.0) - r0 / 3.0;

    return 3;
  }
}

int del3 (double a, double b, double c, double d)
{
  double r0, s, t;
  double p0, q0, delta;

  r0    = b / a;
  s     = c / a;
  t     = d / a;
  p0    = (3.0 * s - r0 * r0) / 3.0;
  q0    = 2.0 * r0 * r0 * r0 / 27.0 - r0 * s / 3.0 + t;
  delta = p0 * p0 * p0 / 27.0 + q0 * q0 / 4.0;

  return delta > 0.0 ? 1 : 3;
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

class SurfaceBuilder
{
  public:
    SurfaceBuilder (const ModelData& in_model_data, int uanz, int vanz) : model_data (in_model_data)
    {
      Init (uanz, vanz);
    }

    void Convert (DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
    {
      out_vertices.reserve (vertices.size ());
      out_primitives.reserve (100); //???

      int p_in = -10;

      size_t first = 0;

      for (size_t i=0; i < vertices.size (); i++)
      {
        const Vertex& vertex = vertices [i];

        if (p_in != vertex.attr)
        {
          if (p_in != -1)
          {
            size_t count = i - first;

            if (count >= 3)
            {
              DrawPrimitive primitive;

              primitive.type  = PrimitiveType_TriangleStrip;
              primitive.first = first;
              primitive.count = count;

              out_primitives.push_back (primitive);
            }
          }

          first = i;
          p_in  = vertex.attr;
        }

        DrawVertex out_vertex;

        copy (vertex.position, out_vertex.position);
        copy (vertex.normal, out_vertex.normal);

        out_vertices.push_back (out_vertex);
      }

      size_t count = vertices.size () - first;

      if (count >= 3)
      {
        DrawPrimitive primitive;

        primitive.type  = PrimitiveType_TriangleStrip;
        primitive.first = first;
        primitive.count = count;

        out_primitives.push_back (primitive);
      }
    }

  private:
    typedef std::vector<Vertex> VertexArray;

    struct _OLD_
    {
      double  p,q,r;

      _OLD_ () : p (0), q (0), r (0) {}
    };

    struct Last
    {
      double ly, ty;
      double lp1, lp2;
      double tp1, tp2;
    };

  private:

    const ModelData&  model_data;
    VertexArray       vertices;
    Last              last;
        //сделать локальными!!!!
    double            e1, e3, e, ei [3];
    short             ok;
    double            f0, f1, f2, f3, dm, p1, p2, ymax;

    int def_ok (double x, double y, _OLD_& old)
    {
      double qq, p, q, r;

      if (y == last.ty)
      {
        p1 = last.tp1;
        p2 = last.tp2;
      }
      else if (y == last.ly)
      {
        p1 = last.lp1;
        p2 = last.lp2;
      }
      else
      {
        p1 = div_2 (e1, e, y);

        if (ok == 3)
        {
          p2 = div_2 (e3, -e, y);
        }
        else
        {
          p2 = div_2 (e1, -e, y);
        }

        last.ly  = last.ty;
        last.lp1 = last.tp1;
        last.lp2 = last.tp2;
        last.ty  = y;
        last.tp1 = p1;
        last.tp2 = p2;
      }

      p = p1 * sin2 (x+PI/4.) + p2 * cos2 (x+PI/4.);

      def_coef (y, p);

      if (sol3 (f3, f2, f1, f0, ei) == 1)
        return 0;

      if ( (x>PI/4.) && (x<3.*PI/4.))
      {
        qq = std::max (ei [0], std::max (ei [1], ei [2]));
      }
      else
      {
        if      (ei [0] < 0.) qq = std::min (ei [1], ei [2]);
        else if (ei [1] < 0.) qq = std::min (ei [0], ei [2]);
        else                  qq = std::min (ei [1], ei [0]);
      }

      qq = sqrt (fabs (qq));
      r  = qq * sin (y);
      q  = qq * cos (y);

      old.p = p;
      old.q = q;
      old.r = r;

      return 1;
    }

    void def_coef (double y, double p0)
    {
      double u1, u2, u3, u4;

      u1 = model_data.B * cos2 (y) + model_data.C * sin2 (y);
      u2 = model_data.B * model_data.B * cos2 (y) + model_data.C * model_data.C * sin2 (y);
      u3 = 2 * model_data.h - model_data.A * p0 * p0;
      u4 = model_data.A * model_data.A * p0 * p0 * p0 - 2 * model_data.h * model_data.A * p0 + 2. * model_data.g * model_data.mx;
      f3 = -u1 * u1 * u2;
      f2 = (2 * u2 * u3 - model_data.A * model_data.A * p0 * p0 * u1) * u1;
      f1 = u2 * (4. * model_data.mx * model_data.mx - u3 * u3) - 2.*model_data.A * p0 * u1 * u4;
      f0 = - u4 * u4;
    }

    double div_2 (double smin, double smax, double y)
    {
      int ii;
      double s;

      for (ii=0; ii < 1000; ii++)
      {
        s = (smin + smax)/2;

        def_coef (y, s);

        if (del3 (f3, f2, f1, f0) == 1) smax = s;
        else                            smin = s;

        if (fabs (smax - smin) < EPS)
          return smin;
      }

      return 0.0;
    }

    void env (double y, double x, Vertex& vertex)
    {
      double z, z1, z2, z3;
      double pp, qq, rr;

      x = x * PI;
      y = y * PI * ymax;

      _OLD_ old;

      if(def_ok(x, y, old))
      {
        vertex.position.x = old.p * dm;
        vertex.position.y = old.q * dm;
        vertex.position.z = old.r * dm;
        pp                = old.p * old.p;
        qq                = old.q * old.q;
        rr                = old.r * old.r;
        z1                = -((model_data.A * pp + model_data.B * qq + model_data.C * rr)/2. - model_data.h) / model_data.mx;
        z2                = model_data.A * model_data.A * pp + model_data.B * model_data.B * qq + model_data.C * model_data.C * rr;
        z3                = -(z2 * z1-model_data.A * model_data.g * old.p) * model_data.mx;
        vertex.normal.x   = model_data.A * (model_data.g * z1 * model_data.mx * model_data.mx-old.p * (model_data.A * model_data.mx * model_data.mx * z1 * z1 + z3));
        vertex.normal.y   = model_data.B * old.q * (model_data.B * model_data.mx * model_data.mx * (1.-z1 * z1)-z3);
        vertex.normal.z   = model_data.C * old.r * (model_data.C * model_data.mx * model_data.mx * (1.-z1 * z1)-z3);
        z                 = sqrt (vertex.normal.x * vertex.normal.x + vertex.normal.y * vertex.normal.y + vertex.normal.z * vertex.normal.z);
        vertex.normal.x   = vertex.normal.x/z;
        vertex.normal.y   = vertex.normal.y/z;
        vertex.normal.z   = vertex.normal.z/z;

        hls2rgb3f ((-z1 + 8.0) * 30, 0.2, 0.9, &vertex.dummy.x, 1);

        vertex.dummy.x = .4;
        vertex.dummy.y = .4;
        vertex.dummy.z = .4;
        vertex.attr    = 1;
      }
      else
      {
        vertex.position.x = 0.;
        vertex.position.y = 0.;
        vertex.position.z = 0.;
        vertex.normal.x   = 0.1;
        vertex.normal.y   = 0.1;
        vertex.normal.z   = 0.1;
        vertex.dummy.x    = 1.0;
        vertex.dummy.y    = 1.0;
        vertex.dummy.z    = 1.0;
        vertex.attr       = 0;
      }
    }

    void Init (int uanz, int vanz)
    {
      vertices.reserve (uanz * vanz * 4);

      memset (ei, 0, sizeof ei);

      dm       = 1.2;
      ymax     = 2;
      last.tp1 = last.tp2 = last.lp1 = last.lp2 = last.ly = last.ty = -1;

      ok = sol3 (model_data.A * model_data.A, 0., -2 * model_data.h * model_data.A, 2. * model_data.g * model_data.mx, ei);

      if (ok == 1)
      {
        e1 = ei [0];
      }
      else if (ok==3)
      {
        e1 = std::max (std::max (ei [0], ei [1]), ei [2]);
        e3 = std::min (std::min (ei [0], ei [1]), ei [2]);

        if (4. * model_data.mx * model_data.mx < pow ((2. * model_data.h - model_data.A * e3 * e3), 2.))
        {
          ok=2;
        }
      }

      e = sqrt (2. * (model_data.h + fabs (model_data.mx)) / model_data.A);

      int  in, i, mn=50;
      double uu,vv;
      Vertex sixpack, copy_six, copy_copy;

      size_t print_step = uanz / 10000;

      if (!print_step)
        print_step++;

      for (int u=0; u<uanz; u++)
      {
        if (!(u % print_step))
          printf ("\rProgress: %.2f%%", (float)u / uanz * 100.f);

        in = 0;

        for (int v=0;v<(vanz);v++)
        {
          for (int swap=0; swap<2; swap++)
          {
            uu = (double)(u+swap)/((double)(uanz));
            vv = (double)(v)/((double)(vanz));

            switch(in)
            {
              case 0:
              case 1:
              {
                env(uu,vv, sixpack);

                if (sixpack.attr != 0.)
                {
                   sixpack.attr = in;

                   vertices.push_back(sixpack);
                }
                else
                {
                  in++;

                  for (i=0; i<mn; i++)
                  {
                    vv = vv - 1./mn/vanz;

                    env (uu, vv, sixpack);

                    if (sixpack.attr)
                    {
                      sixpack.attr = in;

                      vertices.push_back (sixpack);

                      break;
                    }
                  }
                }

                break;
              }
              case 10:
              {
                if (swap==0)
                {
                  env(uu,vv, copy_copy);

                  if (copy_copy.attr)
                    in=11;
                }
                break;
              }
              case 11:
              {
                env (uu, vv, copy_six);

                if (copy_six.attr)
                {
                  in = 0;
                  vv = vv - (mn+1)/mn/vanz;
                  uu = uu-1./uanz;

                  for (i=-1; i<mn; i++)
                  {
                    vv=vv+1./mn/vanz;

                    env (uu,vv, sixpack);

                    if(sixpack.attr)
                    {
                      sixpack.attr = in;

                      vertices.push_back (sixpack);

                      break;
                    }
                  }

                  vv = (v - (mn + 1) / mn)/((double)(vanz));
                  uu = uu + 1. / uanz;

                  for(i=-1; i<mn; i++)
                  {
                    vv = vv + 1. / mn / (vanz-1);

                    env (uu,vv, sixpack);

                    if (sixpack.attr)
                    {
                      sixpack.attr = in;

                      vertices.push_back(sixpack);

                      break;
                    }
                  }

                  sixpack = copy_copy;

                  sixpack.attr = in;

                  vertices.push_back (sixpack);

                  sixpack = copy_six;

                  sixpack.attr = in;

                  vertices.push_back(sixpack);
                }
                else
                {
                  in = 10;
                }
              }
            }
          }

          if (in == 1 || in == 2)
          {
            in = 10;
          }
        }
      }

      printf ("\rProgress: 100.00%%\n");
    }
};

}

/*
    Загрузка модели из файла
*/

void LoadModelData (const char* file_name, ModelData& model_data)
{
  FILE* file = fopen (file_name, "rt");

  if (!file)
  {
    printf ("File '%s' not found\n", file_name);
    return;
  }

  if (fscanf (file, "A=%lf, B=%lf, C=%lf, h=%lf, g=%lf, mx=%lf, my=%lf, mz=%lf, ini=%d",
              &model_data.A, &model_data.B, &model_data.C, &model_data.h,&model_data.g,
              &model_data.mx, &model_data.my, &model_data.mz, &model_data.ini) != 9)
  {
    printf ("Error at read model data\n");
    return;
  }
}

/*
    Построение мега-поверхности
*/

void BuildMegaSurface (const ModelData& model_data, int uanz, int vanz, DrawVertexArray& out_vertices, DrawPrimitiveArray& out_primitives)
{
  SurfaceBuilder (model_data, uanz, vanz).Convert (out_vertices, out_primitives);
}

}
