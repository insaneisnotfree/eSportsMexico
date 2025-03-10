<?php
/**
 * Archivo para desplegar las formas que controlan la inscripci�n a las diferentes modalidades
 * la funci�n de inscripci�n es la principal que revisa si la modalidad actual es v�lida en
 * la temporada activa, en caso de que no est� activa despliega el mensaje y regresa a la vista de la modalidad
 * Si la modalidad actual es v�lida en la temporada activa, revisa si es individual o por equipos
 * y despliega las opciones correspondintes.

/**
 * funci�n que revisa el estado de la modalidad que se est� visitando y si es v�lida en la temporada activa
 * despliega las opciones de acuerdo al m�ximo de jugadores para los equipos.
 */
function inscripcion( ) {
  global $user;
  $tid = arg( 2 );
  
  if (!($modalidad = term_is_active($tid)))
    return t('Esta modalidad no esta activa en esta temporada.');
  
  if($modalidad->identificador == '')
    return t('El identificador de este juego no esta configurado.</br>Por favor contacta a un administrador.');
  
  $profile_node = content_profile_load('profile', $user->uid);
  if ($profile_node->{$modalidad->identificador}['0']['value'] == '') {
    return t('Para registrarte primero debes dar de alta tu identificador online en tu perfil de usuario. Puedes editarlo haciendo click en este link:</br>') . l('Editar Perfil', 'user/'. $user->uid .'/edit/profile');
  }
  
  $tiene_equipo = tiene_equipo( $user->uid, $modalidad->id_Modalidad_Juego_Temporada );
  if ($tiene_equipo == TIENE_EQUIPO)
    return t('Ya perteneces a un equipo de esta modalidad. Para poder inscribirte en otro equipo debes salir primero del equipo al que perteneces actualmente.');
  
  //Si el jugador cumple con todos los requisitos se despliega la forma para crear un nuevo equipo.
  if( !function_exists("node_object_prepare")) {
     include_once(drupal_get_path('module', 'node') . '/node.pages.inc');
  }
  $type = 'team';
  
  // Initialize settings:
  $node = array('uid' => $user->uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $type, 'language' => '', 'id_Modalidad_Juego_Temporada' => $modalidad->id_Modalidad_Juego_Temporada);
  
  return drupal_get_form($type .'_node_form', $node);
}