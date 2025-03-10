<?php
/**
 * Funci�n para administrar las invitaciones y jugadores de un equipo
 */
function team_administrar_activos() {
  global $user;
  $equipo = node_load( arg( 1 ) );  //carga el objeto nodo o equipo

  $jugadores = db_query('SELECT * FROM {eSM_Jugador } AS j
                        INNER JOIN {eSM_Integrantes_Equipo } AS ie ON j.{ id_Jugador } = ie.{ id_Jugador }
                        INNER JOIN {eSM_Equipo } AS e ON ie.{ nid_Equipo } = e.{ nid_Equipo }
                        WHERE e.{ nid_Equipo } = %d AND ie.{ Estado } = %d',
                        $equipo->nid, ACEPTADO);
  
  //Despliega los jugadores
  $count = 0;
  $checkboxes = array ();
  
  //Despliega los jugadores 
  while ($jugador = db_fetch_object( $jugadores ) ) {
    if ($jugador->uid != $user->uid) {
      ++ $count;
      $account = user_load( array( 'uid' => $jugador->uid ) );
      $form['Solicitudes'][ $jugador->id_Jugador ][ 'nombre' ] = array (
        '#value' => $account->name,
      );
    
      $form['Solicitudes'][ $jugador->id_Jugador ][ 'mensaje' ] = array (
        '#value' => $jugador->Mensaje,
      );
      
      $checkboxes[$jugador->id_Jugador] = '';
    }
  }
  
  if ($count == 0) {
    $form['mensaje'] = array (
      '#value' => t('Eres el �nico integrante de tu equipo en este momento.'),
    );
  }
  else {
    $form[ 'checkboxes' ] = array (
      '#type' => 'checkboxes',
      '#options' => $checkboxes,
    );
    $form[ 'submit' ] = array (
      '#type' => 'submit',
      '#value' => t( 'Update Roster' ),
    );
  
    $form[ 'nid_equipo' ] = array (
      '#type' => 'hidden',
      '#value' => $equipo->nid,
    );
  }
  
  $form['#theme'] = 'team_active';
  
  return $form;
}
/**
 * Submit para la la forma de jugadores activos
 * expulsa a los jugadores seleccionados en la forma
 */
function team_administrar_activos_submit( $form, $form_state ) {
  global $user;
  
  $nid_Equipo = $form_state['values'][ 'nid_equipo' ];
  $count = 0; //Contador para mostrar el n�mero de jugadores expulsados

  foreach( element_children($form_state['values']['checkboxes']) as $jugador ) { 
    if ($form_state['values']['checkboxes'][$jugador] != 0) {
      ++$count;
        db_query('UPDATE {eSM_Integrantes_Equipo }
                 SET Estado = %d
                 WHERE nid_Equipo = %d AND id_Jugador = %d',
                 RECHAZADO, $nid_Equipo, $jugador);
        //Actualiza el estado de la tabla de jugador
        db_query ('UPDATE {eSM_Jugador }
                  SET Estado = %d
                  WHERE id_Jugador = %d',
                  AGENTE_LIBRE, $jugador);
      
      //Notifica al ususario que ha sido expulsado del equipo
      rules_invoke_event('usuario_expulsado', $user, user_load(array('uid' => $jugador->uid)), node_load(array('nid' => $nid_Equipo)));
    }
  }

  if ($count > 0)
    drupal_set_message( "Has expulsado ". $count ." jugadores." );
  else
    drupal_set_message('No has seleccionado ning�n jugador para expulsar.', 'error');
}
/**
 * Funci�n para administrar las solicitudes de registro de los jugadores
 */
function team_administrar_solicitudes( ) {
  $equipo = node_load(arg(1));  //carga el objeto nodo o equipo
  
  //Carga todos los jugadores que solicitaron entrar en el equipo.
  $jugadores = db_query('SELECT * FROM {eSM_Jugador } AS j
                        INNER JOIN {eSM_Integrantes_Equipo } AS ie ON j.{ id_Jugador } = ie.{ id_Jugador }
                        INNER JOIN {eSM_Equipo } AS e ON ie.{ nid_Equipo } = e.{ nid_Equipo }
                        WHERE e.{ nid_Equipo } = %d AND ie.{ Estado } = %d',
                        $equipo->nid, SOLICITA);
  
  $count = 0;
  $checkboxes = array ();
  
  //Despliega los jugadores 
  while ($jugador = db_fetch_object( $jugadores ) ) {
    ++ $count;
    $account = user_load( array( 'uid' => $jugador->uid ) );
    $form['Solicitudes'][ $jugador->id_Jugador ][ 'nombre' ] = array (
      '#value' => $account->name,
    );
  
    $form['Solicitudes'][ $jugador->id_Jugador ][ 'mensaje' ] = array (
      '#value' => $jugador->Mensaje,
    );
    
    $checkboxes[$jugador->id_Jugador] = '';
  }
  
  if ($count == 0) {
    $form['mensaje'] = array (
      '#value' => t('No hay solicitudes nuevas en este momento.'),
    );
  }
  else {
    $form[ 'checkboxes' ] = array (
      '#type' => 'checkboxes',
      '#options' => $checkboxes,
    );
    //TODO: Agregar un bot�n para rechazar las solicitudes
    $form[ 'submit' ] = array (
      '#type' => 'submit',
      '#value' => t( 'Aceptar Jugadores Seleccionados.' ),
    );
  
    $form[ 'nid_equipo' ] = array (
      '#type' => 'hidden',
      '#value' => $equipo->nid,
    );
  }
  
  $form['#theme'] = 'team_solicitudes';
  
  return $form;
}
/**
 * Valida las solicitudes que se est�n intentando aprobar
 */
function team_administrar_solicitudes_validate($form, $form_state) {
  $nid_Equipo = $form_state['values'][ 'nid_equipo' ];
  //Contador para mostrar el n�mero de solicitudes aceptados
  $count = 0; 
  //Verifica que el jugador que est� por ser aprovado no pertenezca a un equipo
  foreach( element_children($form_state['values']['checkboxes']) as $jugador ) { 
    if ($form_state['values']['checkboxes'][$jugador] != 0) {
      ++$count;
      $account = db_fetch_object(db_query('SELECT * FROM {eSM_Jugador}
                                         WHERE id_Jugador = %d',
                                         $jugador));
      
      if ($account->Estado == TIENE_EQUIPO) {
        $account = user_load(array('uid' => $jugador->uid));
        form_set_error('checkboxes', t('El jugador '. $account->name .' ya pertenece a otro equipo.'));
      }
    }
  }
  $num_players = _team_num_players($nid_Equipo);
  $node = node_load(array('nid' => $nid_Equipo));
  $modalidad = get_modalidad($node->id_Modalidad_Juego_Temporada);
  if (($num_players + $count) >= $modalidad->Maximo_Jugadores)
    form_set_error('checkboxes', t('No puedes aprobar tantos jugadores porque sobrepasa el m�ximo permitido.'));
}
/**
 * Submit para la la forma de jugadores solicitantes
 */
function team_administrar_solicitudes_submit($form, $form_state) {
  global $user;
  $nid_Equipo = $form_state['values'][ 'nid_equipo' ];
  $count = 0; //Contador para mostrar el n�mero de solicitudes aceptados

  foreach( element_children($form_state['values']['checkboxes']) as $jugador ) { 
    if ($form_state['values']['checkboxes'][$jugador] != 0) {
      ++$count;
      
      db_query('UPDATE {eSM_Integrantes_Equipo}
               SET Estado = %d
               WHERE nid_Equipo = %d AND id_Jugador = %d',
               ACEPTADO, $nid_Equipo, $jugador);
      //Actualiza el estado de la tabla de jugador
      db_query ('UPDATE {eSM_Jugador}
                SET Estado = %d
                WHERE id_Jugador = %d',
                TIENE_EQUIPO, $jugador);
      
      //Notifica al ususario que ha sido aceptado en el equipo
      $uid = db_result(db_query('SELECT uid FROM {eSM_Jugador} 
                                WHERE id_Jugador = %d',
                                $jugador));
      rules_invoke_event('usuario_aceptado', $user, user_load(array('uid' => $uid)), node_load(array('nid' => $nid_Equipo)));
    }
  }

  if ($count > 0)
    drupal_set_message( "Has aprobado ". $count ." solicitudes." );
  else
    drupal_set_message('No has seleccionado ninguna solicitud para aprovar.', 'error');
}
/**
 * Funci�n para administrar las invitaciones y jugadores de un equipo
 */
function team_administrar_invitados( ) {
  $equipo = node_load( arg( 1 ) );  //carga el objeto nodo o equipo
  
  //Pone un recuadro con la lista de jugadores a los que se ha invitado
  $output = '<fieldset class="collapsible"><legend>Jugadores Invitados</legend>';
  
  //Carga todos los jugadores invitados en el equipo.
  $jugadores = db_query('SELECT * FROM {eSM_Jugador } AS j
                        INNER JOIN {eSM_Integrantes_Equipo } AS ie ON j.{ id_Jugador } = ie.{ id_Jugador }
                        INNER JOIN {eSM_Equipo }  AS e ON ie.{ nid_Equipo } = e.{ nid_Equipo }
                        WHERE e.{ nid_Equipo } = %d AND ie.{ Estado } = %d',
                        $equipo->nid, INVITADO);
  
  $count = 0;
  //Despliega los jugadores 
  while ( $jugador = db_fetch_object( $jugadores ) ) {
    $account = user_load( array( 'uid' => $jugador->uid ) );
    $rows[] = $account->name;
    ++$count;
  }
  
  if ($count == 0) 
    $output = '<div class="form_item">'. t('No has invitado todavia a ningun jugador.') .'</div>';
  else
    $output .= theme('table', $header, $rows);
    
  $output .= '</fielset>';
  
  /*TODO: Agregar un espacio para poder invittar a los amigos del capit�n directamente desde esta forma
  if(module_exists('user_relationships_api')) {
    //Agrega un espacio con la lista de los amigos del usuario para poder invitarlos directamente desde esta forma.
    $output .= '<fieldset class="collapsible collapsed"><legend>Amigos</legend>';
    $output .= '</fieldset>';
  }*/
  
  //Agrega un espacio con la lista de agentes libres para poder invitar directamente desde esta forma.
  $output .= '<fieldset class="collapsible"><legend>Agentes Libres</legend>';
  $output .= _team_agente_libre_lista($equipo->id_Modalidad_Juego_Temporada);
  $output .= '</fieldset>';
  
  return $output;
}