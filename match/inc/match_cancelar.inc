<?php
/**
 * Despliega una forma de confirmación para asegurarnos
 * de que no se ha pulsado el link de aceptar por error
 */
function match_cancelar() {
  return confirm_form($form, 'Seguro quieres cancelar este desafio?', 'node/'. arg(1), 'No es posible deshacer esta accion', 'Si, cancelar desafio', 'Regresar');
}
/**
 * Valida que el desafio se pueda cancelar
 */
function match_cancelar_validate($form, &$form_state) {
  global $user;
  $node = node_load(array('nid' => arg(1)));
  
  //Revisa que el estado del desafio tenga el estado de creado y manda error en caso contrario
  if (($node->Estado != CREADO) && ($node->Estado != CONFLICTO))
		form_set_error($form, 'El desafio no admite esta operacion');
}
/**
 * Cancela el desafio:
 * 	- Actualiza el estado del desafío en la base de datos
 * 	- Envía notificaciones al capitán del equipo contrario
 */
function match_cancelar_submit($form, &$form_state) {
	//Revisa que no se hayan colocado errores en la forma
	
	if (count($errores = drupal_get_messages('error')) > 0) {
		//Despliega los errores asignados a la forma
		
		foreach($errores['error'] AS $index => $error) {
			if (!(strncmp('warning', $error, 7) == 0))
			drupal_set_message($error, 'error');
		}
		
		drupal_goto('node/'. arg(1));
	}
	else {
		//No hay errores, por lo tanto cancela el reto
		$node = node_load(array('nid' => arg(1)));
		//Primero obtenemos los datos de los capitanes de los equipos  
		$home_team = node_load(array('nid' => $node->home_team));
		$away_team = node_load(array('nid' => $node->away_team));
		$home_capitan = user_load(array('uid' => $home_team->uid));
		$away_capitan = user_load(array('uid' => $away_team->uid));
		
		if ($node->Estado == CREADO) {
			//Envía la notificación de que el match no fue aceptado al equipo creador
			rules_invoke_event('desafio_rechazado', $away_capitan, $home_capitan, $node);
		}
		elseif ($node->Estado == PROGRAMADO) {
			//Envía la notificación de que se canceló el match como decisión de un juez
			rules_invoke_event('desafio_cancelado', $home_capitan, $node);
			rules_invoke_event('desafio_cancelado', $away_capitan, $node);
		}
		//Cambia el estado del match en la base de datos
		db_query('UPDATE {eSM_Reto} SET Estado = %d WHERE nid_Reto = %d', CANCELADO, $node->nid);
		
		drupal_set_message('El desafio ha sido cancelado.');
		drupal_goto($_GET['q']);
	}
}