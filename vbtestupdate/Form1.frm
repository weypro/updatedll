VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "updatetest"
   ClientHeight    =   3570
   ClientLeft      =   120
   ClientTop       =   450
   ClientWidth     =   4890
   LinkTopic       =   "Form1"
   ScaleHeight     =   3570
   ScaleWidth      =   4890
   StartUpPosition =   3  '´°¿ÚÈ±Ê¡
   Begin VB.CommandButton Command1 
      Caption         =   "vc dll"
      Height          =   735
      Left            =   2280
      TabIndex        =   0
      Top             =   1800
      Width           =   1215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function Init Lib "downdll.dll" (ByVal strTargetURL As String) As Long
Private Declare Function GetNewVersion Lib "downdll.dll" (ByVal strVersion As String) As Long
Private Declare Function GetNewURL Lib "downdll.dll" (ByVal strURL As String) As Long
Private Declare Function GetNewTargetFile Lib "downdll.dll" () As Long
Private Declare Sub CleanUp Lib "downdll.dll" ()

Private Sub Command1_Click()
    Dim version As String * 50
    Dim url As String * 50
    Dim targeturl As String
    
    targeturl = "https://wey.xsyds.cn/update.txt"
    Init (targeturl)
    GetNewVersion (version)
    GetNewURL (url)
    GetNewTargetFile
    CleanUp
    
    Print version
    Print url
End Sub
