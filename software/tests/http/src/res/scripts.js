    var image = new Image();

    function imgNewName()
    {
      var val = Math.random()*(1000.99);
      var res = "jpg-image?d=" + val.toString();
      return res;
    }

    function loadHandler()
    {
      console.log( "load handler" );
      $( "#img" ).attr( "src", image.src );
      image.src = imgNewName();
    }

    function documentReady()
    {
      console.log( "document ready" );
      var curName = imgNewName();
      image.onload = loadHandler;
      image.src = curName;
      //setInterval( loadHandler, 1000 );
    }
    
    console.log( "before setting handler" );
    $( document ).ready( documentReady );
    console.log( "after setting handler" );
