Attribute VB_Name = "Module1"
Sub AutoSizeTitles()
    Dim sld As Slide
    Dim shp As Shape

    For Each sld In ActivePresentation.Slides
        For Each shp In sld.Shapes
            If shp.Type = msoPlaceholder Then
                If shp.PlaceholderFormat.Type = ppPlaceholderTitle Then
                    shp.TextFrame.AutoSize = ppAutoSizeShapeToFitText
                End If
            End If
        Next shp
    Next sld
End Sub

