###################################################################################################
#�������������� ��������� ����������
###################################################################################################
export.LIBS                 := funner.common funner.system
export.IMPORTS              := ../common/link.static.mak
export.win32.IMPORTS        := ../../extern/pthreads_win32/link.static.mak
export.win32.LIBS           := user32
export.macosx.LINK_FLAGS    := -mmacosx-version-min=10.4
export.linux.LIBS           := dl pthread
