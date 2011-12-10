<?php
/**
 * Función para mostrar la vista de los torneos de cada modalidad
 */
function torneo_modalidades () {
  $tid = arg(2);
  $op = arg(4);
  $modalidad = term_is_active($tid);
  //Colocamos el título de la pestaña
  drupal_set_title(t('Torneos de '. $modalidad->Nombre));
  
  //Agregamos la vista para mostrar los torneos
  if ($op == "last") {
    $content = views_embed_view('proximos_torneos_modalidad', 'block_2');
  }
  else {
    $content = views_embed_view('proximos_torneos_modalidad', 'default');
  }
  return $content;
}