    var image = new Image();
    var timeout;
    var camX;
    var camY;

    function imgNewName()
    {
      var val = Math.random()*(1000.99);
      var res = "jpg-image?d=" + val.toString();
      return res;
    }

    function onInterval()
    {
      var auto = $( '#autorefresh' ).prop( "checked" );
      if ( auto )
      {
        var name = imgNewName();
        image.src = name;
      }
    }

    function onTimeout( tm )
    {
      clearTimeout( timeout );
      onInterval()
    }

    function loadHandler()
    {
      console.log( "load handler" );
      $( "#img" ).attr( "src", image.src );
      //timeout = setTimeout( onTimeout, 200 );
      onInterval()
    }

    function documentReady()
    {
      camX = 0;
      camY = 0;
      quality = 0;

      console.log( "document ready" );
      var name = imgNewName();
      image.onload = loadHandler;
      image.src = name;
      //setInterval( onInterval, 1000 );
      timeout = setTimeout( onTimeout, 1000 );


      $( "#send" ).click( function() {
        var data = {}
        data.a = 123;
        data.b = "Hello!";

        var posting = $.post( "dataUrl", data );
      });



      $( "#slider-vertical" ).slider({
        orientation: "vertical",
        range: "min",
        min: -90,
        max: 90,
        value: 0,
        slide: function( event, ui ) {
          camY = ui.value;
          $( "#dirY" ).val( ui.value );
          cameraPosSend();
          }
        });
        $( "#dirY" ).val( $( "#slider-vertical" ).slider( "value" ) );


        $( "#slider-horizontal" ).slider({
          orientation: "horizontal",
          range: "min",
          min: -90,
          max: 90,
          value: 0,
          slide: function( event, ui ) {
            camX = ui.value;
            $( "#dirX" ).val( ui.value );
            cameraPosSend();
            }
          });
          $( "#dirX" ).val( $( "#slider-horizontal" ).slider( "value" ) );

        $( "#slider-quality" ).slider({
          orientation: "horizontal",
          range: "min",
          min: 1,
          max: 100,
          value: 70,
          slide: function( event, ui ) {
            quality = ui.value;
            $( "#quality" ).val( ui.value );
            qualitySend();
            }
          });
          $( "#quality" ).val( $( "#slider-quality" ).slider( "value" ) );
          
        $( '#autorefresh' ).prop( "checked", true );
        $( '#autorefresh' ).click( onInterval );
        $( '#force-refresh-image' ).click( forceRefreshImage );
        $( '#post-camera-orientation' ).click( cameraPosSend );
        $( '#light' ).click( lightEn )
    }

    function forceRefreshImage()
    {
      $( '#autorefresh' ).prop( "checked", false );
      var name = imgNewName();
      var img = $( '#img' ).attr( "src", name );
      $( '#result' ).text( 'Force image refresh is invoked' );
    }

    function cameraPosSend()
    {
      var data = {}
      data.camX = camX;
      data.camY = camY;

      var posting = $.post( "chassis", data );
      $( '#result' ).text( 'Camera position: ' + camX.toString() + ", " + camY.toString() );
    }

    function qualitySend()
    {
      var data = {}
      data.quality = quality;

      var posting = $.post( "jpg-image", data );
      $( '#result' ).text( 'Image quality: ' + quality.toString() );
    }

    function lightEn()
    {
      var data = {}
      var en = $( '#light' ).prop( "checked" );
      if ( en )
        data.light = 1;
      else
        data.light = 0;

      var posting = $.post( "chassis", data );
      var stri;
      if ( data.light )
        stri = "Light is on";
      else
        stri = "Light is off";
      $( '#result' ).text( stri );
    }

    console.log( "before setting handler" );
    $( document ).ready( documentReady );
    console.log( "after setting handler" );
