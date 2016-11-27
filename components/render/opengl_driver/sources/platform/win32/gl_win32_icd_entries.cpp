#include "shared.h"

namespace
{

/*
    Константы
*/

//таблица имён ICD-точек входа
const char* ICD_ENTRY_NAMES [] = {
  "glNewList",        /* 0 */
  "glEndList",        /* 1 */
  "glCallList",       /* 2 */
  "glCallLists",        /* 3 */
  "glDeleteLists",      /* 4 */
  "glGenLists",       /* 5 */
  "glListBase",       /* 6 */
  "glBegin",          /* 7 */
  "glBitmap",         /* 8 */
  "glColor3b",        /* 9 */
  "glColor3bv",       /* 10 */
  "glColor3d",        /* 11 */
  "glColor3dv",       /* 12 */
  "glColor3f",        /* 13 */
  "glColor3fv",       /* 14 */
  "glColor3i",        /* 15 */
  "glColor3iv",       /* 16 */
  "glColor3s",        /* 17 */
  "glColor3sv",       /* 18 */
  "glColor3ub",       /* 19 */
  "glColor3ubv",        /* 20 */
  "glColor3ui",       /* 21 */
  "glColor3uiv",        /* 22 */
  "glColor3us",       /* 23 */
  "glColor3usv",        /* 24 */
  "glColor4b",        /* 25 */
  "glColor4bv",       /* 26 */
  "glColor4d",        /* 27 */
  "glColor4dv",       /* 28 */
  "glColor4f",        /* 29 */
  "glColor4fv",       /* 30 */
  "glColor4i",        /* 31 */
  "glColor4iv",       /* 32 */
  "glColor4s",        /* 33 */
  "glColor4sv",       /* 34 */
  "glColor4ub",       /* 35 */
  "glColor4ubv",        /* 36 */
  "glColor4ui",       /* 37 */
  "glColor4uiv",        /* 38 */
  "glColor4us",       /* 39 */
  "glColor4usv",        /* 40 */
  "glEdgeFlag",       /* 41 */
  "glEdgeFlagv",        /* 42 */
  "glEnd",          /* 43 */
  "glIndexd",         /* 44 */
  "glIndexdv",        /* 45 */
  "glIndexf",         /* 46 */
  "glIndexfv",        /* 47 */
  "glIndexi",         /* 48 */
  "glIndexiv",        /* 49 */
  "glIndexs",         /* 50 */
  "glIndexsv",        /* 51 */
  "glNormal3b",       /* 52 */
  "glNormal3bv",        /* 53 */
  "glNormal3d",       /* 54 */
  "glNormal3dv",        /* 55 */
  "glNormal3f",       /* 56 */
  "glNormal3fv",        /* 57 */
  "glNormal3i",       /* 58 */
  "glNormal3iv",        /* 59 */
  "glNormal3s",       /* 60 */
  "glNormal3sv",        /* 61 */
  "glRasterPos2d",      /* 62 */
  "glRasterPos2dv",     /* 63 */
  "glRasterPos2f",      /* 64 */
  "glRasterPos2fv",     /* 65 */
  "glRasterPos2i",      /* 66 */
  "glRasterPos2iv",     /* 67 */
  "glRasterPos2s",      /* 68 */
  "glRasterPos2sv",     /* 69 */
  "glRasterPos3d",      /* 70 */
  "glRasterPos3dv",     /* 71 */
  "glRasterPos3f",      /* 72 */
  "glRasterPos3fv",     /* 73 */
  "glRasterPos3i",      /* 74 */
  "glRasterPos3iv",     /* 75 */
  "glRasterPos3s",      /* 76 */
  "glRasterPos3sv",     /* 77 */
  "glRasterPos4d",      /* 78 */
  "glRasterPos4dv",     /* 79 */
  "glRasterPos4f",      /* 80 */
  "glRasterPos4fv",     /* 81 */
  "glRasterPos4i",      /* 82 */
  "glRasterPos4iv",     /* 83 */
  "glRasterPos4s",      /* 84 */
  "glRasterPos4sv",     /* 85 */
  "glRectd",          /* 86 */
  "glRectdv",         /* 87 */
  "glRectf",          /* 88 */
  "glRectfv",         /* 89 */
  "glRecti",          /* 90 */
  "glRectiv",         /* 91 */
  "glRects",          /* 92 */
  "glRectsv",         /* 93 */
  "glTexCoord1d",       /* 94 */
  "glTexCoord1dv",      /* 95 */
  "glTexCoord1f",       /* 96 */
  "glTexCoord1fv",      /* 97 */
  "glTexCoord1i",       /* 98 */
  "glTexCoord1iv",      /* 99 */
  "glTexCoord1s",       /* 100 */
  "glTexCoord1sv",      /* 101 */
  "glTexCoord2d",       /* 102 */
  "glTexCoord2dv",      /* 103 */
  "glTexCoord2f",       /* 104 */
  "glTexCoord2fv",      /* 105 */
  "glTexCoord2i",       /* 106 */
  "glTexCoord2iv",      /* 107 */
  "glTexCoord2s",       /* 108 */
  "glTexCoord2sv",      /* 109 */
  "glTexCoord3d",       /* 110 */
  "glTexCoord3dv",      /* 111 */
  "glTexCoord3f",       /* 112 */
  "glTexCoord3fv",      /* 113 */
  "glTexCoord3i",       /* 114 */
  "glTexCoord3iv",      /* 115 */
  "glTexCoord3s",       /* 116 */
  "glTexCoord3sv",      /* 117 */
  "glTexCoord4d",       /* 118 */
  "glTexCoord4dv",      /* 119 */
  "glTexCoord4f",       /* 120 */
  "glTexCoord4fv",      /* 121 */
  "glTexCoord4i",       /* 122 */
  "glTexCoord4iv",      /* 123 */
  "glTexCoord4s",       /* 124 */
  "glTexCoord4sv",      /* 125 */
  "glVertex2d",       /* 126 */
  "glVertex2dv",        /* 127 */
  "glVertex2f",       /* 128 */
  "glVertex2fv",        /* 129 */
  "glVertex2i",       /* 130 */
  "glVertex2iv",        /* 131 */
  "glVertex2s",       /* 132 */
  "glVertex2sv",        /* 133 */
  "glVertex3d",       /* 134 */
  "glVertex3dv",        /* 135 */
  "glVertex3f",       /* 136 */
  "glVertex3fv",        /* 137 */
  "glVertex3i",       /* 138 */
  "glVertex3iv",        /* 139 */
  "glVertex3s",       /* 140 */
  "glVertex3sv",        /* 141 */
  "glVertex4d",       /* 142 */
  "glVertex4dv",        /* 143 */
  "glVertex4f",       /* 144 */
  "glVertex4fv",        /* 145 */
  "glVertex4i",       /* 146 */
  "glVertex4iv",        /* 147 */
  "glVertex4s",       /* 148 */
  "glVertex4sv",        /* 149 */
  "glClipPlane",        /* 150 */
  "glColorMaterial",      /* 151 */
  "glCullFace",       /* 152 */
  "glFogf",         /* 153 */
  "glFogfv",          /* 154 */
  "glFogi",         /* 155 */
  "glFogiv",          /* 156 */
  "glFrontFace",        /* 157 */
  "glHint",         /* 158 */
  "glLightf",         /* 159 */
  "glLightfv",        /* 160 */
  "glLighti",         /* 161 */
  "glLightiv",        /* 162 */
  "glLightModelf",      /* 163 */
  "glLightModelfv",     /* 164 */
  "glLightModeli",      /* 165 */
  "glLightModeliv",     /* 166 */
  "glLineStipple",      /* 167 */
  "glLineWidth",        /* 168 */
  "glMaterialf",        /* 169 */
  "glMaterialfv",       /* 170 */
  "glMateriali",        /* 171 */
  "glMaterialiv",       /* 172 */
  "glPointSize",        /* 173 */
  "glPolygonMode",      /* 174 */
  "glPolygonStipple",     /* 175 */
  "glScissor",        /* 176 */
  "glShadeModel",       /* 177 */
  "glTexParameterf",      /* 178 */
  "glTexParameterfv",     /* 179 */
  "glTexParameteri",      /* 180 */
  "glTexParameteriv",     /* 181 */
  "glTexImage1D",       /* 182 */
  "glTexImage2D",       /* 183 */
  "glTexEnvf",        /* 184 */
  "glTexEnvfv",       /* 185 */
  "glTexEnvi",        /* 186 */
  "glTexEnviv",       /* 187 */
  "glTexGend",        /* 188 */
  "glTexGendv",       /* 189 */
  "glTexGenf",        /* 190 */
  "glTexGenfv",       /* 191 */
  "glTexGeni",        /* 192 */
  "glTexGeniv",       /* 193 */
  "glFeedbackBuffer",     /* 194 */
  "glSelectBuffer",     /* 195 */
  "glRenderMode",       /* 196 */
  "glInitNames",        /* 197 */
  "glLoadName",       /* 198 */
  "glPassThrough",      /* 199 */
  "glPopName",        /* 200 */
  "glPushName",       /* 201 */
  "glDrawBuffer",       /* 202 */
  "glClear",          /* 203 */
  "glClearAccum",       /* 204 */
  "glClearIndex",       /* 205 */
  "glClearColor",       /* 206 */
  "glClearStencil",     /* 207 */
  "glClearDepth",       /* 208 */
  "glStencilMask",      /* 209 */
  "glColorMask",        /* 210 */
  "glDepthMask",        /* 211 */
  "glIndexMask",        /* 212 */
  "glAccum",          /* 213 */
  "glDisable",        /* 214 */
  "glEnable",         /* 215 */
  "glFinish",         /* 216 */
  "glFlush",          /* 217 */
  "glPopAttrib",        /* 218 */
  "glPushAttrib",       /* 219 */
  "glMap1d",          /* 220 */
  "glMap1f",          /* 221 */
  "glMap2d",          /* 222 */
  "glMap2f",          /* 223 */
  "glMapGrid1d",        /* 224 */
  "glMapGrid1f",        /* 225 */
  "glMapGrid2d",        /* 226 */
  "glMapGrid2f",        /* 227 */
  "glEvalCoord1d",      /* 228 */
  "glEvalCoord1dv",     /* 229 */
  "glEvalCoord1f",      /* 230 */
  "glEvalCoord1fv",     /* 231 */
  "glEvalCoord2d",      /* 232 */
  "glEvalCoord2dv",     /* 233 */
  "glEvalCoord2f",      /* 234 */
  "glEvalCoord2fv",     /* 235 */
  "glEvalMesh1",        /* 236 */
  "glEvalPoint1",       /* 237 */
  "glEvalMesh2",        /* 238 */
  "glEvalPoint2",       /* 239 */
  "glAlphaFunc",        /* 240 */
  "glBlendFunc",        /* 241 */
  "glLogicOp",        /* 242 */
  "glStencilFunc",      /* 243 */
  "glStencilOp",        /* 244 */
  "glDepthFunc",        /* 245 */
  "glPixelZoom",        /* 246 */
  "glPixelTransferf",     /* 247 */
  "glPixelTransferi",     /* 248 */
  "glPixelStoref",      /* 249 */
  "glPixelStorei",      /* 250 */
  "glPixelMapfv",       /* 251 */
  "glPixelMapuiv",      /* 252 */
  "glPixelMapusv",      /* 253 */
  "glReadBuffer",       /* 254 */
  "glCopyPixels",       /* 255 */
  "glReadPixels",       /* 256 */
  "glDrawPixels",       /* 257 */
  "glGetBooleanv",      /* 258 */
  "glGetClipPlane",     /* 259 */
  "glGetDoublev",       /* 260 */
  "glGetError",       /* 261 */
  "glGetFloatv",        /* 262 */
  "glGetIntegerv",      /* 263 */
  "glGetLightfv",       /* 264 */
  "glGetLightiv",       /* 265 */
  "glGetMapdv",       /* 266 */
  "glGetMapfv",       /* 267 */
  "glGetMapiv",       /* 268 */
  "glGetMaterialfv",      /* 269 */
  "glGetMaterialiv",      /* 270 */
  "glGetPixelMapfv",      /* 271 */
  "glGetPixelMapuiv",     /* 272 */
  "glGetPixelMapusv",     /* 273 */
  "glGetPolygonStipple",    /* 274 */
  "glGetString",        /* 275 */
  "glGetTexEnvfv",      /* 276 */
  "glGetTexEnviv",      /* 277 */
  "glGetTexGendv",      /* 278 */
  "glGetTexGenfv",      /* 279 */
  "glGetTexGeniv",      /* 280 */
  "glGetTexImage",      /* 281 */
  "glGetTexParameterfv",    /* 282 */
  "glGetTexParameteriv",    /* 283 */
  "glGetTexLevelParameterfv", /* 284 */
  "glGetTexLevelParameteriv", /* 285 */
  "glIsEnabled",        /* 286 */
  "glIsList",         /* 287 */
  "glDepthRange",       /* 288 */
  "glFrustum",        /* 289 */
  "glLoadIdentity",     /* 290 */
  "glLoadMatrixf",      /* 291 */
  "glLoadMatrixd",      /* 292 */
  "glMatrixMode",       /* 293 */
  "glMultMatrixf",      /* 294 */
  "glMultMatrixd",      /* 295 */
  "glOrtho",          /* 296 */
  "glPopMatrix",        /* 297 */
  "glPushMatrix",       /* 298 */
  "glRotated",        /* 299 */
  "glRotatef",        /* 300 */
  "glScaled",         /* 301 */
  "glScalef",         /* 302 */
  "glTranslated",       /* 303 */
  "glTranslatef",       /* 304 */
  "glViewport",       /* 305 */
  "glArrayElement",     /* 306 */
  "glBindTexture",      /* 307 */
  "glColorPointer",     /* 308 */
  "glDisableClientState",   /* 309 */
  "glDrawArrays",       /* 310 */
  "glDrawElements",     /* 311 */
  "glEdgeFlagPointer",    /* 312 */
  "glEnableClientState",    /* 313 */
  "glIndexPointer",     /* 314 */
  "glIndexub",        /* 315 */
  "glIndexubv",       /* 316 */
  "glInterleavedArrays",    /* 317 */
  "glNormalPointer",      /* 318 */
  "glPolygonOffset",      /* 319 */
  "glTexCoordPointer",    /* 320 */
  "glVertexPointer",      /* 321 */
  "glAreTexturesResident",  /* 322 */
  "glCopyTexImage1D",     /* 323 */
  "glCopyTexImage2D",     /* 324 */
  "glCopyTexSubImage1D",    /* 325 */
  "glCopyTexSubImage2D",    /* 326 */
  "glDeleteTextures",     /* 327 */
  "glGenTextures",      /* 328 */
  "glGetPointerv",      /* 329 */
  "glIsTexture",        /* 330 */
  "glPrioritizeTextures",   /* 331 */
  "glTexSubImage1D",      /* 332 */
  "glTexSubImage2D",      /* 333 */
  "glPopClientAttrib",    /* 334 */
  "glPushClientAttrib",   /* 335 */
};

const size_t ICD_ENTRIES_COUNT = sizeof (ICD_ENTRY_NAMES) / sizeof (*ICD_ENTRY_NAMES);

/*
    Карта имён точек входа
*/

class IcdEntryMap
{
  public:
///Конструктор
    IcdEntryMap ()
    {
      for (size_t i=0; i<ICD_ENTRIES_COUNT; i++)
        entries [ICD_ENTRY_NAMES [i]] = i;
    }
    
///Получение индекса точки входа
    int FindIndex (const char* name)
    {
      if (!name)
        return -1;

      Map::iterator iter = entries.find (name);

      if (iter == entries.end ())
        return -1;

      return (int)iter->second;
    }

  private:
    typedef stl::hash_map<stl::hash_key<const char*>, size_t> Map;

  private:
    Map entries;
};

typedef common::Singleton<IcdEntryMap> IcdEntryMapSingleton;

}

namespace render
{

namespace low_level
{

namespace opengl
{

namespace windows
{

/*
    Получение индекса точки входа в таблице ICD-драйвера
*/

int get_icd_table_index (const char* name)
{
  return IcdEntryMapSingleton::Instance ()->FindIndex (name);
}

}

}

}

}
