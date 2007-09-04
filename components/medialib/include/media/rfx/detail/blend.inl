/*
    BlendFunction
*/

inline BlendFunction::BlendFunction (BlendEquation in_equation, BlendArgument in_arg1, BlendArgument in_arg2)
  : equation (in_equation)
{
  argument [0] = in_arg1;
  argument [1] = in_arg2;
}

/*
    Создание функций смешивания
*/

//src_color * src_alpha + dst_color * (1 - src_alpha)
inline BlendFunction make_blend_filter   ()
{
  return BlendFunction (BlendEquation_Add, BlendArgument_SourceAlpha, BlendArgument_InverseSourceAlpha);
}

//src_color + dst_color
inline BlendFunction make_blend_additive ()
{
  return BlendFunction (BlendEquation_Add, BlendArgument_One, BlendArgument_One);
}

//src_color
inline BlendFunction make_blend_solid ()
{
  return BlendFunction (BlendEquation_Add, BlendArgument_One, BlendArgument_Zero);
}
