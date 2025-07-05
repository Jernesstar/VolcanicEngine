!include "MUI2.nsh"

!define APP_NAME "VolcanicEngine"
!define EDITOR_EXE "Editor.exe"
!define RUNTIME_EXE "Runtime.exe"
!define MUI_ICON "assets\icons\favicon.ico"
!define MUI_UNICON "assets\icons\favicon.ico"

Name "VolcanicEngine"
OutFile "${APP_NAME}Installer.exe"
InstallDir "$PROGRAMFILES\${APP_NAME}"
RequestExecutionLevel admin

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

Section "Install"
  SetOutPath "$INSTDIR"
  File /r "app\*.*"
  File /r "imgui.ini"
  SetOutPath "$INSTDIR\Magma\assets"
  File /r "assets\"
  CreateShortCut "$DESKTOP\Magma.lnk" "$INSTDIR\${EDITOR_EXE}" "" "$INSTDIR\Magma\assets\icons\favicon.ico" 0 SW_SHOWNORMAL "" "Launch the Magma Editor"
  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd

!define env_hklm 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'

Section "Set Environment Variable"
  WriteRegExpandStr ${env_hklm} "VOLC_PATH" "$INSTDIR"
  WriteRegExpandStr ${env_hklm} "VOLC_RUNTIME" "$INSTDIR\Runtime.exe"
  System::Call 'user32::SendMessageA(i 0xffff, i ${WM_WININICHANGE}, i 0, t "Environment")'
SectionEnd

Section "Uninstall"
  Delete "$DESKTOP\MyApp.lnk"
  DeleteRegValue ${env_hklm} "VOLC_PATH"
  DeleteRegValue ${env_hklm} "VOLC_RUNTIME"
  System::Call 'user32::SendMessageA(i 0xffff, i ${WM_WININICHANGE}, i 0, t "Environment")'
  Delete "$INSTDIR\${EDITOR_EXE}"
  Delete "$INSTDIR\${RUNTIME_EXE}"
  Delete "$INSTDIR\imgui.ini"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir /r "$INSTDIR\Magma"
  RMDir "$INSTDIR"
SectionEnd
