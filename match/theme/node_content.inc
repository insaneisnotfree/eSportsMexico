<?php

function theme_match_content($node) {
  global $user;
  //Añade el archivo css donde viene la personalización de la vista del desafio
  drupal_add_css(drupal_get_path('module', 'match') . '/css/match.css');

  //Verifica si el usuario actual es capitán de alguno de los dos equipos
  if (team_user_is_team_capitan($user->uid, $node->nid_Equipo_Local) || $away_capitan = team_user_is_team_capitan($user->uid, $node->nid_Equipo_Visitante))
    $is_capitan = TRUE;
  
  //Obtiene los datos de los equipos participantes en el desafío
  $home_team = node_load($node->nid_Equipo_Local);
  $away_team = node_load($node->nid_Equipo_Visitante);
  
  //Obtiene la modalidad a la que pertenece el desafío
  $modalidad = get_modalidad($node->id_Modalidad_Juego_Temporada);
  
  //Convertir el estado numérico en texto
  switch ($node->Estado) {
    case CREADO:
      $estado = 'Por Confirmar';
      break;
    case PROGRAMADO:
      $estado = 'Programado';
      break;
    case CANCELADO:
      $estado = 'Cancelado';
      break;
    case COMPLETADO:
      $estado = 'Terminado';
      break;
    case CONFLICTO:
      $estado = 'En conflicto';
      break;
    case ELIMINADO:
      $estado = 'Eliminado';
      break;
    case TABLERO:
      return;
  }
  $header = array();
  
  //Primera columa del contenido
  $rows['1'][] = array('data' => $node->title, 'class' => 'header left'); 
  $rows['1'][] = array('data' => 'Parámetros', 'class' => 'header right');
    
    //Despliega los logos de los equipos y la palabra vs
    $content['2'][] = array('data' => theme('team_pane', $home_team, 'match'), 'class' => 'team');
    $content['2'][] = array('data' => t(' VS. '), 'class' => 'vs');
    $content['2'][] = array('data' => theme('team_pane', $away_team, 'match'), 'class' => 'team');
    //Despliega los nombres de los equipos como link al perfil de los mismos
    $content['3'][] = array('data' => l($home_team->title, 'node/'. $home_team->nid), 'class' => 'team_title');
    $content['3'][] = ' ';
    $content['3'][] = array('data' => l($away_team->title, 'node/'. $away_team->nid), 'class' => 'team_title');
    //Despliega el ránking de los equipos
    $content['4'][] = 'Ranking';
    $content['4'][] = ' ';
    $content['4'][] = 'Ranking';
    $content['5'][] = ranking_get_ranking_equipo($home_team->nid, $node->id_Modalidad_Juego_Temporada);
    $content['5'][] = ' ';
    $content['5'][] = ranking_get_ranking_equipo($away_team->nid, $node->id_Modalidad_Juego_Temporada);
    //Despliega la puntuación de los equipos
    $content['6'][] = 'Puntos';
    $content['6'][] = ' ';
    $content['6'][] = 'Puntos';
    $content['7'][] = $node->Puntuacion_Inicial_Equipo_Local;
    $content['7'][] = ' ';
    $content['7'][] = $node->Puntuacion_Inicial_Equipo_Visitante;
    //Despliega la racha que tiene el equipo
    $content['8'][] = 'Racha';
    $content['8'][] = ' ';
    $content['8'][] = 'Racha';
    $content['9'][] = $home_team->Racha;
    $content['9'][] = ' ';
    $content['9'][] = $away_team->Racha;
    //Despliega el % de victorias tienen los equipos
    $content['10'][] = '% Victorias';
    $content['10'][] = ' ';
    $content['10'][] = '% Victorias';
    $content['11'][] = $home_team->Victorias;
    $content['11'][] = ' ';
    $content['11'][] = $away_team->Victorias;
  
  $rows['2'][] = array('data' => theme('table', $header, $content), 'class' => 'content_left');
  
  //Segunda columna de contenido
  $content = array();
  $content['2'][] = array('data' => '<span>MatchID: </span>'. $node->nid, 'class' => 'matchid');
  $content['3'][] = array('data' => '<span>Modalidad: </span>'. $modalidad->Nombre, 'class' => 'matchid');
  $content['4'][] = array('data' => '<span>Fecha: </span>'. format_date($node->Fecha_Inicio, 'large', ''), 'class' => 'match_date');
  $content['5'][] = array('data' => '<span>No. Rondas: </span>'. $node->Numero_Rondas, 'class' => 'match_round');
  $content['6'][] = array('data' => '<span>Estado: </span>'. $estado, 'class' => 'match_round');
  
  //Genera un espacio para separar los links
  $content['7'][] = array('data' => 'Acciones:', 'class' => 'subheader');
  
  $actions = '<div class="actions">';
  //Si el desafio está en estado creado y el usuario es capitán del equipo retado despliega un link para aceptar el reto
  if (($node->Estado == CREADO) && $away_capitan) {
    $actions .= l('Aceptar Desafio', 'node/'. $node->nid . '/accept');
    $actions .= l('Rechazar Desafio', 'node/'. $node->nid . '/cancel');
  }
  
  //Si el desafio está en estado programado despliega un link para reportar el resultado
  if ($node->Estado == PROGRAMADO) {
    if ($is_capitan && ($node->Fecha_Inicio + 60*30 <= time())) {
      $actions .= l('Reportar Resultado', 'node/'. $node->nid . '/report');
    }
    elseif ($is_capitan) {
      $actions .= t('Podrás reportar el resultado de este desafío 30min después de la fecha de inicio programada.');
    }
  }
  //Si el desafio está en estado conflicto despliega un link para subir las pruebas
  if (($node->Estado == CONFLICTO) && $is_capitan) {
    $actions .= l('Enviar Pruebas', 'node/'. $node->nid . '/proof');
  }
  $actions .= '</div>';
  $content['8'][] = $actions;
  
  $rows['2'][] = array('data' => theme('table', $header, $content), 'class' => 'content_right');
  
  $output = theme('table', $header, $rows, array('id' => 'Match'));
  
  //Añade una tabla nueva para poner los resultados
  $rows = array();

  $content = array();
  //Si es un desafio completado pone el nombre del equipo ganador
  if ($node->Estado == COMPLETADO) {
    if ($node->nid_Equipo_Local == $node->ganador)
      $winner = $home_team;
    else
      $winner = $away_team;
    
    $content['2'][] = array('data' => '<div class="winer">Resultado: '. $winner->title .' gana!</div>', 'colspan' => '5');
  }
  else {
    $content['2'][] = array('data' => '<div class="winer">Desafío en Progreso </div>', 'colspan' => '5');
  }
  
  $rows['4'][] = theme('table', $header, $content);

  $output .= theme('table', $header, $rows, array('id' => 'Result'));
  
  return $output;
}