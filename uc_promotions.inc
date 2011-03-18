<?php
/**
 * @file
 * Promotions administration menu items.
 */

/**
 * Display created promotions.
 */
function uc_promotions_get_promos(){
	$output = t("<p>Qui di seguito trovi la lista delle promozioni create:</p>");
	$output .= _get_promos();
	return $output;
}

/**
 * Display a form to create a promotion.
 */
function uc_promotions_create_promo(){
	$output = t("<p>Compila la form che segue per creare una nuova promozione:</p>");
	$output .= drupal_get_form('uc_promotions_form');
	return $output;
}

/**
 * Display a form to modify a promotion.
 */
function uc_promotions_edit_promo($promo_id = ''){
	$output = t("<p>Compila la form che segue per modificare una promozione esistente:</p>");
	$output .= drupal_get_form('uc_promotions_form', $promo_id);
	return $output;
}

/*
 * Returns the form to add a promotion
 */
function uc_promotions_form(&$form_state, $data = NULL){
	$form['nid'] = array(
		'#title' => t('Prodotto in promozione'),
		'#description' => t('Attenzione: per visualizzare un prodotto nella lista, il prodotto stesso deve essere NON PUBBLICATO'),
		'#type' => 'select',
		'#options' => _get_products(),
		'#required' => true,
		'#multiple' => false,
		'#default_value' => $data['nid'],
	);
	
	$form['price_limit'] = array(
		'#title' => t('Limite prezzo'),
		'#description' => t('Indica il limite che l\'acquirente deve raggiungere per attivare la promozione'),
		'#type' => 'textfield',
		'#required' => true,
		'#size' => 30,
		'#default_value' => $data['price_limit'],
	);
	
	$form['domain'] = array(
		'#title' => t('Dominio su cui si applica la promozione'),
		'#description' => t('Indica il dominio/minisito su cui si applica la promozione'),
		'#type' => 'textfield',
		'#required' => false,
		'#size' => 30,
		'#default_value' => $data['domain'],
	);
	
	$form['status'] = array(
		'#title' => t('Attivo'),
		'#description' => t('Indica se la promozione è attiva oppure no'),
		'#type' => 'select',
		'#options' => array('' => '', '1' => 'si', '0' => 'no'),
		'#required' => true,
		'#default_value' => $data['status'],
	);
	
	$form['promoid'] = array(
		'#title' => t('Promoid'),
		'#type' => 'hidden',
		'#default_value' => $data['promoid'],
	);
	
	if($data['promoid']){
		$form['del'] = array(
			'#title' => t('Cancella'),
			'#description' => t('Permette di cancellare la promozione'),
			'#type' => 'select',
			'#options' => array('0'=>'no', '1'=>'si'),
			'#required' => false,
		);
	}
	
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Invia i dati'),
	);
	
	return $form;
}

/*
 * Validates form values 
 */
function uc_promotions_form_validate($form, &$form_state){
	if(!is_numeric($form_state['values']['price_limit'])){
		form_set_error('', t("Attenzione il campo Limite prezzo accetta solo valori numerici"));
	}
}

/*
 * Submits form values 
 */
function uc_promotions_form_submit($form, &$form_state){
	$v = $form_state['values'];
	
	//Delete a promotion
	if($v['del'] == 1){
		$sql = "DELETE FROM {uc_promotionstions} WHERE promoid = %d";
		if(db_query($sql, $v['promoid'])){
			$form_state['redirect'] = 'admin/store/uc_promotionstions';
			drupal_set_message(t('Promozione eliminata con successo'), $type = 'status');
		}
	}else{
		//Create or update a promotion
		$update = isset($v['promoid']) && $v['promoid'] != '' ? 'promoid' : array();

		if(drupal_write_record('uc_promotionstions', $form_state['values'], $update)) {
			$form_state['redirect'] = 'admin/store/uc_promotionstions';
			drupal_set_message(t('Promozione creata/aggiornata con successo'), $type = 'status');
		}else{
			drupal_set_message(t('Errore durante l\'inserimento/modifica dei dati'), $type = 'error');
		}
	}
}


/* 
 * Returns a list of promotions
 */
function _get_promos(){
	$sql = "SELECT * FROM {uc_promotionstions}";
	$result = db_query($sql);
	
	$headers = array();
  $headers[]= t('Promo-id');
  $headers[]= t('Prodotto in promozione');
  $headers[]= t('Limite prezzo');
  $headers[]= t('Dominio');
	$headers[]= t('Status');
	$headers[]= t('Azioni');
	
	$rows = array();
	while($data = db_fetch_object($result)){
		$content = array();
		$product = node_load($data->nid);
		
		$content[]= $data->promoid;
		$content[]= l($product->title, 'node/'.$product->nid);
		$content[]= '&euro; '.number_format($data->price_limit, 2);
		$content[]= $data->domain;
		$content[]= $data->status == '1' ? t('active') : t('not active');
		$content[]= l(t('edit'), 'admin/store/uc_promotions/'.$data->promoid.'/edit');
		$rows[]=$content;
	}
	 
	return theme_table($headers, $rows);
}

/*
 * Returns active products
 */
function _get_products(){
	$sql = "SELECT * FROM {node} WHERE type = '%s' AND status = %d ORDER BY '%s'";
	$result = db_query(db_rewrite_sql($sql), "product", 0, 'nid');
	
	$results = array("" => "");
	while($data = db_fetch_object($result)){
		$results[$data->nid] = '['.$data->nid.'] '.$data->title;
	}
	
	return $results;
}