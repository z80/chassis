    var image = new Image();
    var timeout;

    function imgNewName()
    {
      var val = Math.random()*(1000.99);
      var res = "jpg-image?d=" + val.toString();
      return res;
    }

    function onTimeout( tm )
    {
      clearTimeout( timeout );
      var name = imgNewName();
      image.src = name;
    }

    function loadHandler()
    {
      console.log( "load handler" );
      $( "#img" ).attr( "src", image.src );
      timeout = setTimeout( onTimeout, 200 );
    }

    function onPostCamXy()
    {
      var x = $( "#slider-horizontal" ).slider( "value" );
    }

    function documentReady()
    {
      /*
      console.log( "document ready" );
      var name = imgNewName();
      image.onload = loadHandler;
      image.src = name;
      //setInterval( loadHandler, 1000 );
      timeout = setTimeout( onTimeout, 1000 );
      */


      $( "#send" ).click( function() {
        var data = {}
        data.a = 123;
        data.b = "Hello!";

        var url = window.location.origin;
        var posting = $.post( "dataUrl", data );


        /*
        // Put the results in a div
        posting.done(function( data ) {
          var content = $( data ).find( "#content" );
          $( "#result" ).empty().append( content );
        });
        */
        //window.location.origin
      });



      $( "#slider-vertical" ).slider({
        orientation: "vertical",
        range: "min",
        min: -90,
        max: 90,
        value: 0,
        slide: function( event, ui ) {
          $( "#dirY" ).val( ui.value );
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
            $( "#dirX" ).val( ui.value );
            }
          });
          $( "#dirX" ).val( $( "#slider-horizontal" ).slider( "value" ) );
    }

    console.log( "before setting handler" );
    $( document ).ready( documentReady );
    console.log( "after setting handler" );
