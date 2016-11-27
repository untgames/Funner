

function CreateMultiTextLine()
  local MultiLine={}
  MultiLine.Node=Scene.Node.Create()
  local TextLines={} --текстовые линии в которые будет производится вывожд текста
  
  local MaxTextLength=nil
  MultiLine.StringLengthTerminal=" "
  MultiLine.Scale=vec3(10,10,1)
  MultiLine.Font="data/fonts/stylo.xfont"
  MultiLine.Color=vec4(1,1,1,1)
  
  ------------------------------------------------------------------------------------------------------
  -- методы
  ------------------------------------------------------------------------------------------------------
  
  local Height=0
  
  local LineAlignmentX, LineAlignmentY = Scene.TextLineAlignment.Center, Scene.TextLineAlignment.Center
  
  MultiLine.Redraw=function ()
    local scale=MultiLine.Scale
    local font=MultiLine.Font
    local color=MultiLine.Color
    Height=0
    local hstep=math.abs(scale.y)
    ForEachInTable(TextLines,function(line)
      line.Position=vec3(0,Height,0)
      line.Scale=scale
      line.Font=font
      line.Color=color
      line:SetAlignment( LineAlignmentX, LineAlignmentY)
      Height=Height-hstep
    end)
  end
  
  MultiLine.SetMaxTextLength=function(L)
    MaxTextLength=L
    MultiLine.SetText(MultiLine.Text)
  end
  
  MultiLine.SetAlignment=function( AlignmentX, AlignmentY)
    LineAlignmentX, LineAlignmentY = AlignmentX, AlignmentY
    MultiLine.Redraw()
  end
  
  
  MultiLine.SetText=function(text)
    if not(type(text)=="string") then 
      text=tostring(text) 
    end
    MultiLine.Text=text
    -- анализ полученного текста и генерация строк при необходимости
    local strings={}
    if MaxTextLength then
      if MaxTextLength>0 then
        strings=CutToSmallerStrings (text, MultiLine.StringLengthTerminal, MaxTextLength, true)
      end
    else
      strings[1]=text
    end
    -- устаревшие строки отсоединяются от узла
    ForEachInTable(TextLines, function(line)
      line:Unbind()
    end)
    -- создаются новые строки с текстом и присоединяются к узлу
    TextLines={}
    local TextLineIndex=0
    ForEachInTable(strings, function(str)
      TextLineIndex=TextLineIndex+1
      local line=Scene.TextLine.Create()
      line:BindToParent(MultiLine.Node)
      line.Text=str
      TextLines[TextLineIndex]=line
    end)
    MultiLine.Redraw()
  end
  
  return MultiLine
end
